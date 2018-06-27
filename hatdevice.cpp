#include "hatdevice.h"
#include "ui_hatdevice.h"
#include "mainwindow.h"
//#include "childwindow.h"

HatDevice::HatDevice(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HatDevice)
{
    ui->setupUi(this);
    hatInterface = new HatInterface();

    mHistListSize = 50;

    tmrCheckStatus = new QTimer(this);
    mUseGetStatus = true;
    mStopOnStart = false;
    functionGroup = new QActionGroup(this);
    buffer = NULL;

    mAddress = -1;
    mNumHats = 0;
    mScanOptions = 0;
    mTriggerType = TRIG_RISING_EDGE;
    mAiResolution = 12;
    mCurFunction = UL_AIN;
    mTriggered = false;
    mBackgroundScan = true;
    mQueueEnabled = false;

    ui->teShowValues->setFont(QFont ("Courier", 8));
    ui->teShowValues->setStyleSheet("QTextEdit { background-color : white; color : blue; }" );
    ui->lblRateReturned->setFont(QFont ("Courier", 8));
    ui->lblRateReturned->setStyleSheet("QLabel { background-color : white; color : blue; }" );
    ui->lblBufferSize->setFont(QFont ("Courier", 8));
    ui->lblBufferSize->setStyleSheet("QLabel { background-color : white; color : grey; }" );
    ui->lblStatus->setStyleSheet("QLabel { color : blue; }" );
    ui->lblInfo->setStyleSheet("QLabel { color : blue; }" );

    connect(tmrCheckStatus, SIGNAL(timeout()), this, SLOT(checkStatus()));
    connect(ui->cmdGo, SIGNAL(clicked(bool)), this, SLOT(runSelectedFunction()));
    connect(ui->cmdStop, SIGNAL(clicked(bool)), this, SLOT(stopCmdClicked()));

    connect(ui->AiPlot->xAxis, SIGNAL(rangeChanged(QCPRange)),
            ui->AiPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->AiPlot->yAxis, SIGNAL(rangeChanged(QCPRange)),
            ui->AiPlot->yAxis2, SLOT(setRange(QCPRange)));
    connect(ui->rbAutoScale, SIGNAL(clicked(bool)), this, SLOT(replot()));
    connect(ui->rbFullScale, SIGNAL(clicked(bool)), this, SLOT(replot()));

    rbPlotSel[0] = ui->rbPlot0;
    rbPlotSel[1] = ui->rbPlot1;
    rbPlotSel[2] = ui->rbPlot2;
    rbPlotSel[3] = ui->rbPlot3;
    rbPlotSel[4] = ui->rbPlot4;
    rbPlotSel[5] = ui->rbPlot5;
    rbPlotSel[6] = ui->rbPlot6;
    rbPlotSel[7] = ui->rbPlot7;
    for (int i = 0; i<8; i++) {
        connect(rbPlotSel[i], SIGNAL(clicked(bool)), this, SLOT(plotSelect()));
        rbPlotSel[i]->setVisible(false);
    }
    mPlotChan = -1;
    setupPlot(ui->AiPlot, 1);
    ui->AiPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->AiPlot->axisRect()->setRangeZoom(Qt::Vertical);
    ui->AiPlot->axisRect()->setRangeDrag(Qt::Vertical);
    ui->AiPlot->replot();
    for (int i = 0; i < 8; i++)
        mPlotList[i] = true;
    showPlotWindow(false);

}

HatDevice::~HatDevice()
{
    delete ui;
}

void HatDevice::keyPressEvent(QKeyEvent *event)
{
    int keyCode = event->key();
    if (keyCode == Qt::Key_Escape) {
        //setTmrRunning(false);
        stopScan();
    }
    if (keyCode == Qt::Key_F6)
        readBuffer();
}

void HatDevice::closeEvent(QCloseEvent *event)
{
    event->accept();
}

void HatDevice::updateParameters()
{
    ChildWindow *parentWindow;
    QString trigString;

    parentWindow = qobject_cast<ChildWindow *>(this->parent());
    mDevName = parentWindow->devName();
    mAddress = parentWindow->devAddress();
    mHatID = parentWindow->devId();
    mScanOptions = parentWindow->scanOptions();
    mTriggerType = parentWindow->triggerType();

    mOptNames = getOptionNames(mScanOptions);
    trigString = getTrigText(mTriggerType);
    ui->lblInfo->setText(QString("Options: %1,  Trigger: %2")
                         .arg(mOptNames).arg(trigString));
    ui->lblStatus->clear();
    this->setWindowTitle(mFuncName + ": " + mDevName);
}

void HatDevice::showQueueConfig()
{
    queueSetup = new QueueDialog(this);
    if (mChanList.count()) {
        queueSetup->setChanList(mChanList);
        queueSetup->setNumQueueElements(mChanList.count());
    }
    connect(queueSetup, SIGNAL(accepted()), this, SLOT(queueDialogResponse()));
    queueSetup->exec();
}

void HatDevice::queueDialogResponse()
{
    mChanList = queueSetup->chanList();
    mQueueEnabled = queueSetup->queueEnabled();

    disconnect(queueSetup);
    delete queueSetup;
    setupQueue();
}

void HatDevice::setupQueue()
{
    unsigned int numElements = mChanList.count();

    mChanCount = numElements;
    int chanAdjust = 0;
    if (numElements)
        chanAdjust = 1;
    ui->spnLowChan->setValue(0);
    ui->spnHighChan->setValue(numElements - chanAdjust);
}

void HatDevice::setUiForFunction()
{
    //ChildWindow *parentWindow;
    //parentWindow = qobject_cast<ChildWindow *>(this->parent());
    //mRange = parentWindow->getCurrentRange();
    bool scanVisible, voltCheckVisible;

    //mChanList.clear();
    //mRangeList.clear();
    mPlot = false;
    scanVisible = false;
    voltCheckVisible = false;
    switch (mCurFunction) {
    case UL_AIN:
        mFuncName = "ulAIn";
        ui->leNumSamples->setText("10");
        break;
    case UL_TIN:
        mFuncName = "ulTIn";
        ui->leNumSamples->setText("10");
        voltCheckVisible = true;
        break;
    case UL_AINSCAN:
        mFuncName = "ulAInScan";
        scanVisible = true;
        mPlot = true;
        ui->leRate->setText("1000");
        ui->leNumSamples->setText("1000");
        ui->leBlockSize->setText("1000");
        break;
    default:
        break;
    }
    ui->fraScan->setVisible(scanVisible);
    ui->chkVolts->setVisible(voltCheckVisible);
    //ui->cmdStop->setEnabled(false);
    showPlotWindow(mPlot);
    this->setWindowTitle(mFuncName + ": " + mDevName);
}

void HatDevice::functionChanged(int utFunction)
{
    mCurFunction = utFunction;
    this->setUiForFunction();
}

void HatDevice::stopCmdClicked()
{
    stopScan();
    return;

    /*
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    uint16_t status;

    funcArgs = "(mAddress, &status, samplesPerChan, timeout, &buffer, bufferSize, samplesRead)\n";
    mStatusTimerEnabled = false;
    nameOfFunc = "118: AInScanRead";
    sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
    mResponse = mcc118_a_in_scan_read(mAddress, &status, 0, 0.0, NULL, 0, NULL);

    argVals = QStringLiteral("(%1, %2, %3, %4, %5, %6, %7)")
            .arg(mAddress)
            .arg(status)
            .arg("0")
            .arg("0.0")
            .arg("NULL")
            .arg("0")
            .arg("NULL");
    mRunning = false; //((status && STATUS_RUNNING) == STATUS_RUNNING);
    tmrCheckStatus->stop();

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    if (mResponse != RESULT_SUCCESS) {
        mMainWindow->setError(mResponse, sStartTime + funcStr);
    } else {
        mMainWindow->addFunction(sStartTime + funcStr);
    }

    nameOfFunc = "118_AInScanStop";
    funcArgs = "(mAddress)\n";
    sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
    mResponse = mcc118_a_in_scan_stop(mAddress);
    argVals = QStringLiteral("(%1)")
            .arg(mAddress);
    ui->lblInfo->setText(nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse));
    //ui->cmdStop->setEnabled(mRunning);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    if (mResponse != RESULT_SUCCESS) {
        mMainWindow->setError(mResponse, sStartTime + funcStr);
        return;
    } else {
        mMainWindow->addFunction(sStartTime + funcStr);
    }
    */
}

void HatDevice::runSelectedFunction()
{
    if(mPlot && (ui->stackedWidget->currentIndex() == 0))
        showPlotWindow(mPlot);
    ui->lblInfo->clear();
    ui->lblStatus->clear();
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    switch (mCurFunction) {
    case UL_AIN:
        runAinFunction();
        break;
    case UL_AINSCAN:
        runAInScanFunc();
        break;
    case UL_TIN:
        runTinFunction();
        break;
    default:
        break;
    }
}

void HatDevice::runSetTriggerFunc()
{
    QString trigString;

    mResponse = hatInterface->setTrigger(mHatID, mAddress, mTriggerType);
    ui->lblStatus->setText(hatInterface->getStatus());

    trigString = getTrigText(mTriggerType);
    ui->lblInfo->setText("Trigger type: " + trigString);

}

void HatDevice::runAinFunction()
{
    uint8_t aInChan, aInLastChan;
    int curIndex;
    double data;

    data = 0.0;

    if(!mQueueEnabled) {
        mChanList.clear();
        aInChan = ui->spnLowChan->value();
        aInLastChan = ui->spnHighChan->value();
        mChanCount = (aInLastChan - aInChan) + 1;
        for(int chan = 0; chan < mChanCount; chan++)
            mChanList[chan] = aInChan + chan;
    }
    if(mChanCount < 1) mChanCount = 1;

    //if queue is enabled, mChanCount is set in setupQueue
    mSamplesPerChan = ui->leNumSamples->text().toLong();
    if(mPlot)
        setupPlot(ui->AiPlot, mChanCount);

    if (buffer) {
        delete[] buffer;
        buffer = NULL;
    }

    long long bufSize = mChanCount * mSamplesPerChan;
    mBufSize = bufSize;
    buffer = new double[bufSize];
    memset(buffer, 0.00000001, mBufSize * sizeof(*buffer));

    uint8_t curChan;
    curIndex = 0;
    mRunning = true;
    for (uint32_t sampleNum = 0; sampleNum < mSamplesPerChan; sampleNum++) {
        foreach(curChan, mChanList) {
            mResponse = hatInterface->aInRead(mHatID, mAddress, curChan, mScanOptions, data);
            ui->lblInfo->setText(hatInterface->getStatus());
            if(mResponse == RESULT_SUCCESS) {
                buffer[curIndex] = data;
                curIndex++;
            } else {
                return;
            }
        }
    }

    mRunning = false;
    if(mPlot)
        plotScan(0, 0, mSamplesPerChan);
    else
        printData(0, 0, mSamplesPerChan);
}

void HatDevice::runTinFunction()
{
    uint8_t aInChan, aInLastChan;
    int curIndex;
    double data;
    bool showVolts;

    data = 0.0;
    showVolts = ui->chkVolts->isChecked();

    if(!mQueueEnabled) {
        mChanList.clear();
        aInChan = ui->spnLowChan->value();
        aInLastChan = ui->spnHighChan->value();
        mChanCount = (aInLastChan - aInChan) + 1;
        for(int chan = 0; chan < mChanCount; chan++)
            mChanList[chan] = aInChan + chan;
    }
    if(mChanCount < 1) mChanCount = 1;

    //if queue is enabled, mChanCount is set in setupQueue
    mSamplesPerChan = ui->leNumSamples->text().toLong();
    if(mPlot)
        setupPlot(ui->AiPlot, mChanCount);

    if (buffer) {
        delete[] buffer;
        buffer = NULL;
    }

    long long bufSize = mChanCount * mSamplesPerChan;
    mBufSize = bufSize;
    buffer = new double[bufSize];
    memset(buffer, 0.00000001, mBufSize * sizeof(*buffer));

    uint8_t curChan;
    curIndex = 0;
    mRunning = true;
    for (uint32_t sampleNum = 0; sampleNum < mSamplesPerChan; sampleNum++) {
        foreach(curChan, mChanList) {
            if(showVolts)
                mResponse = hatInterface->aInRead(mHatID, mAddress, curChan, 0, data);
            else
                mResponse = hatInterface->tInRead(mHatID, mAddress, curChan, data);
            ui->lblInfo->setText(hatInterface->getStatus());
            if(mResponse == RESULT_SUCCESS) {
                buffer[curIndex] = data;
                curIndex++;
            } else {
                return;
            }
        }
    }

    mRunning = false;
    if(mPlot)
        plotScan(0, 0, mSamplesPerChan);
    else
        printData(0, 0, mSamplesPerChan);
}

void HatDevice::runAInScanFunc()
{
    int lowChan, highChan;
    uint8_t chanMask;
    int32_t sampsToRead;
    uint32_t bufferSize;
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime, statString;

    if(mHatID != HAT_ID_MCC_118) {
        //so far, only compatible with 118
        ui->lblStatus->setText("Syncronous scan not supported for this device");
        ui->lblInfo->setText("Select a different device or function");
        return;
    }
    bufferSize = 0;

    if (buffer) {
        mResponse = hatInterface->aInScanCleanup(mHatID, mAddress);
        delete[] buffer;
        buffer = NULL;
    }

    if(mScanOptions & OPTS_EXTTRIGGER) {
        runSetTriggerFunc();
        mTriggered = false;
    }
    //backgroundScan = ui->actionBACKGROUND->isChecked();
    mBlockSize = ui->leBlockSize->text().toLongLong();
    if(!mQueueEnabled) {
        mChanList.clear();
        lowChan = ui->spnLowChan->value();
        highChan = ui->spnHighChan->value();
        mChanCount = (highChan - lowChan) + 1;
        for(int chan = 0; chan < mChanCount; chan++)
            mChanList[chan] = lowChan + chan;
    }
    //if queue is enabled, mChanCount is set in setupQueue
    if(mChanCount < 1) mChanCount = 1;

    mSamplesPerChan = ui->leNumSamples->text().toLong();
    double rate = ui->leRate->text().toDouble();
    //double rateRtn;

    chanMask = 0;
    uint8_t curChan;
    foreach(curChan, mChanList)
        chanMask |= (1 << curChan);

    mTotalRead =0;
    ui->lblStatus->clear();
    long long bufSize = mChanCount * mSamplesPerChan;
    if(mPlot)
        setupPlot(ui->AiPlot, mChanCount);

    //mFunctionFlag = (AInScanFlag)mAiFlags;
    if (mStopOnStart) {
        nameOfFunc = "ulAInScanStop";
        funcArgs = "(mDaqDeviceHandle)";
        //sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        //err = ulAInScanStop(mDaqDeviceHandle);
        //argVals = QString("(%1)").arg(mDaqDeviceHandle);
/*
        funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
        if (!err==ERR_NO_ERROR) {
            mMainWindow->setError(err, sStartTime + funcStr);
        } else {
            mMainWindow->addFunction(sStartTime + funcStr);
        }
  */
    }

    mBufSize = bufSize;
    buffer = new double[bufSize];
    memset(buffer, 0.00000001, mBufSize * sizeof(*buffer));

    nameOfFunc = "118: AInScanStart";
    funcArgs = "(mAddress, chanMask, mSamplesPerChan, &rate, mScanOptions)\n";
    sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
    mResponse = mcc118_a_in_scan_start(mAddress, chanMask, mSamplesPerChan, rate, mScanOptions);
    argVals = QStringLiteral("(%1, %2, %3, %4, %5)")
            .arg(mAddress)
            .arg(chanMask)
            .arg(mSamplesPerChan)
            .arg(rate)
            .arg(mScanOptions);
    ui->lblInfo->setText(nameOfFunc + argVals + "   " +
                         mOptNames + QString("  [Error = %1]").arg(mResponse));

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    hatInterface->reportResult(mResponse, sStartTime + funcStr);
    if (mResponse!=RESULT_SUCCESS) {
        mStatusTimerEnabled = false;
    } else {
        mRunning = true;
        mResponse = mcc118_a_in_scan_actual_rate(mChanCount, rate, &mRateReturned);
        ui->lblRateReturned->setText(QString("%1").arg(mRateReturned, 1, 'f', 4, '0'));
        mResponse = mcc118_a_in_scan_buffer_size(mAddress, &bufferSize);
        ui->lblBufferSize->setText(QString("%1").arg(bufferSize));
        if(mBackgroundScan) {
            mStatusTimerEnabled = true;
            tmrCheckStatus->start(500);
        } else {
            uint16_t status;
            double timeout;
            uint32_t sampsReadPerChan;
            timeout = ui->leTimeout->text().toDouble();
            nameOfFunc = "118: AInScanRead";
            funcArgs = "(mAddress, status, mSamplesToRead, timo, buffer, bufSize, numRead)\n";
            sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
            do {
                mResponse = mcc118_a_in_scan_read(mAddress, &status, 0, timeout, NULL, 0, NULL);
                statString = getStatusText(status);
                ui->lblStatus->setText(QString("  Status: %1").arg(statString));
                delay(200);
            } while (status & STATUS_RUNNING);
            argVals = QStringLiteral("(%1, %2, %3, %4, %5, %6, %7)")
                    .arg(mAddress)
                    .arg(status)
                    .arg("0")
                    .arg(timeout)
                    .arg("NULL")
                    .arg("0")
                    .arg("NULL");
            ui->lblInfo->setText(nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse));

            funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
            hatInterface->reportResult(mResponse, sStartTime + funcStr);
            if (mResponse!=RESULT_SUCCESS) {
                //mMainWindow->setError(mResponse, sStartTime + funcStr);
                return;
            }

            sampsToRead = -1;
            sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
            mResponse = mcc118_a_in_scan_read(mAddress, &status, sampsToRead, timeout, buffer, mBufSize, &sampsReadPerChan);
            argVals = QStringLiteral("(%1, %2, %3, %4, %5, %6, %7)")
                    .arg(mAddress)
                    .arg(status)
                    .arg(sampsToRead)
                    .arg(timeout)
                    .arg("buffer")
                    .arg(mBufSize)
                    .arg(sampsReadPerChan);
            ui->lblInfo->setText(nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse));
            statString = getStatusText(status);

            funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
            hatInterface->reportResult(mResponse, sStartTime + funcStr);
            if (mResponse!=RESULT_SUCCESS) {
                //mMainWindow->setError(mResponse, sStartTime + funcStr);
                return;
            }
            if(sampsReadPerChan) {
                ui->lblStatus->setText(QString("%1 samples read  Status: %2").arg(sampsReadPerChan).arg(statString));
                if(mPlot)
                    plotScan(0, 0, sampsReadPerChan);
                else
                    printData(0, 0, sampsReadPerChan);
            }
            stopScan();
        }
    }
}

void HatDevice::checkStatus()
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime, statString;
    QFont goFont = ui->cmdGo->font();
    bool makeBold, loopStatus;
    bool trigWait, overrunDetected;

    if(mHatID != HAT_ID_MCC_118) {
        //so far, only compatible with 118
        ui->lblStatus->setText("Syncronous scan not supported for this device");
        ui->lblInfo->setText("Select a different device or function");
        return;
    }
    uint16_t status;
    double timeout, readTime;
    uint32_t samplesPerChanRead, samplesAvailable;
    timeout = ui->leTimeout->text().toDouble();
    overrunDetected = false;
    readTime = mBlockSize / mRateReturned;
    loopStatus = (readTime > 0.2);

    //check if the scan is triggered - if not, wait here
    if(loopStatus | ((mScanOptions & OPTS_EXTTRIGGER) && !mTriggered)) {
        do {
            mResponse = hatInterface->readAInScanStatus(mHatID, mAddress, status, samplesAvailable);
            ui->lblInfo->setText(hatInterface->getStatus());
            statString = getStatusText(status);
            ui->lblStatus->setText(QString("Total samples read: %1  Status: %2   [Current options: %3]")
                                   .arg(mTotalRead).arg(statString).arg(mOptNames));

            statString = getStatusText(status);
            mTriggered = (status & STATUS_TRIGGERED);
            if((status &
                (STATUS_BUFFER_OVERRUN | STATUS_HW_OVERRUN)) != 0)
                trigWait = false;
            else if(status & STATUS_RUNNING) {
                if(samplesAvailable)
                    trigWait = !mTriggered & ((mBlockSize - samplesAvailable) / mRateReturned < 0.3);
                else
                    trigWait = true;
            } else
                trigWait = false;
            if (mResponse!=RESULT_SUCCESS) {
                //mMainWindow->setError(mResponse, sStartTime + funcStr);
                trigWait = false;
                stopScan();
                return;
            }
            delay(200);
        } while (trigWait);
    }

    if(mRunning) {
        funcArgs = "(mAddress, &status, mBlockSize, timeout, buffer, mBufSize, &samplesPerChanRead)\n";
        nameOfFunc = "118: AInScanRead";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_a_in_scan_read(mAddress, &status, mBlockSize,
            timeout, buffer, mBufSize, &samplesPerChanRead);
        argVals = QString("(%1, %2, %3, %4, %5, %6, %7)")
                .arg(mAddress).arg(status).arg(mBlockSize).arg(timeout)
                .arg("buffer").arg(mBufSize).arg(samplesPerChanRead);
        overrunDetected = (status & (STATUS_BUFFER_OVERRUN | STATUS_HW_OVERRUN));
        if (overrunDetected)
            argVals += " [OVERRUN]";
        statString = getStatusText(status);
        ui->lblInfo->setText(nameOfFunc + argVals + "   " +
                             mOptNames + QString(" [Error = %1]").arg(mResponse));

        funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
        if(mResponse == RESULT_TIMEOUT)
                if(mScanOptions & OPTS_EXTCLOCK) {
                    QString errText = getErrorDescription(mResponse);
                    funcStr.append(" [" + errText + "]");
                    mResponse = RESULT_SUCCESS;
                }
        hatInterface->reportResult(mResponse, sStartTime + funcStr);
        qApp->processEvents();
        if ((mResponse!=RESULT_SUCCESS) | overrunDetected) {
            mStatusTimerEnabled = false;
            //mMainWindow->setError(mResponse, sStartTime + funcStr);
            delay(200);
            stopScan();
            return;
        }
    }

    if(samplesPerChanRead) {
        mPlotSize = samplesPerChanRead;
        mTotalRead += samplesPerChanRead;
        if(mPlot)
            plotScan(0, 0, samplesPerChanRead);
        else
            printData(0, 0, samplesPerChanRead);
    }
    mRunning = (status & STATUS_RUNNING);
    ui->lblStatus->setText(QString("%1 samples read  Status: %2").arg(mTotalRead).arg(statString));
    if(!mRunning) {
        stopScan();
        goFont.setBold(false);
        return;
    }

    makeBold = !ui->cmdGo->font().bold();
    goFont.setBold(makeBold);
    ui->cmdGo->setFont(goFont);
}

void HatDevice::stopScan()
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;

    if(mHatID != HAT_ID_MCC_118) {
        //so far, only compatible with 118
        ui->lblStatus->setText("Syncronous scan not supported for this device");
        ui->lblInfo->setText("Select a different device or function");
        return;
    }

    QFont goFont = ui->cmdGo->font();

    tmrCheckStatus->stop();
    goFont.setBold(false);
    ui->cmdGo->setFont(goFont);

    mResponse = hatInterface->stopAInScan(mHatID, mAddress);
    mStatusTimerEnabled = false;
    if (mResponse == RESULT_SUCCESS) {
        delay(200);
        runReadScanStatus();
    }
}

void HatDevice::runReadScanStatus()
{
    QString statString;
    uint16_t status;
    uint32_t samplesAvailable;

    if(mHatID != HAT_ID_MCC_118) {
        //so far, only compatible with 118
        ui->lblStatus->setText("Syncronous scan not supported for this device");
        ui->lblInfo->setText("Select a different device or function");
        return;
    }

    mResponse = hatInterface->readAInScanStatus(mHatID, mAddress, status, samplesAvailable);
    ui->lblInfo->setText(hatInterface->getStatus());
    statString = getStatusText(status);
    ui->lblStatus->setText(QString("Total samples read: %1  Status: %2   [Current options: %3]")
                           .arg(mTotalRead).arg(statString).arg(mOptNames));

    return;
    if(samplesAvailable) {
        mBlockSize = -1;
        checkStatus();
    }
}

void HatDevice::readBuffer()
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime, statString;
    uint16_t status;
    uint32_t samplesAvailable, samplesPerChanRead;
    double timeout = 0;

    ui->lblInfo->clear();
    ui->lblStatus->clear();
    if(mHatID != HAT_ID_MCC_118) {
        //so far, only compatible with 118
        ui->lblStatus->setText("Syncronous scan not supported for this device");
        ui->lblInfo->setText("Select a different device or function");
        return;
    }

    mResponse = hatInterface->readAInScanStatus(mHatID, mAddress, status, samplesAvailable);
    ui->lblInfo->setText(hatInterface->getStatus());
    statString = getStatusText(status);
    ui->lblStatus->setText(QString("Total samples read: %1  Status: %2   [Current options: %3]")
                           .arg(mTotalRead).arg(statString).arg(mOptNames));
    if(samplesAvailable > 0) {
        //checkStatus();
        funcArgs = "(mAddress, &status, mBlockSize, timeout, buffer, mBufSize, &samplesPerChanRead)\n";
        nameOfFunc = "118: AInScanRead";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_a_in_scan_read(mAddress, &status, mBlockSize,
            timeout, buffer, mBufSize, &samplesPerChanRead);
        argVals = QString("(%1, %2, %3, %4, %5, %6, %7)")
                .arg(mAddress).arg(status).arg(mBlockSize).arg(timeout)
                .arg("buffer").arg(mBufSize).arg(samplesPerChanRead);
        ui->lblInfo->setText(nameOfFunc + argVals + "   " +
                             mOptNames + QString(" [Error = %1]").arg(mResponse));

        funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
        qApp->processEvents();
        if(samplesPerChanRead) {
            mPlotSize = samplesPerChanRead;
            mTotalRead += samplesPerChanRead;
            if(mPlot)
                plotScan(0, 0, samplesPerChanRead);
            else
                printData(0, 0, samplesPerChanRead);
        }
    }
}

void HatDevice::showPlotWindow(bool showIt)
{
    QFrame::Shape frameShape;

    //mPlot = ui->actionVolts_vs_Time->isChecked();
    mPlot = showIt;

    int curIndex = 0;
    frameShape = QFrame::Box;

    if (showIt) {
        curIndex = 1;
        frameShape = QFrame::NoFrame;
    }
    ui->stackedWidget->setFrameShape(frameShape);
    ui->stackedWidget->setCurrentIndex(curIndex);
}

void HatDevice::setupPlot(QCustomPlot *dataPlot, int chanCount)
{
    QColor penColor;
    QPalette brushColor;

    //brushColor = QPalette::background();
    int chanCycle;
    int curChanCount;
    dataPlot->clearGraphs();
    dataPlot->setBackground(brushColor.background());
    dataPlot->axisRect()->setBackground(Qt::white);
    chanCycle = -1;

    if(mPlotChan == -1)
        curChanCount = chanCount;
    else
        curChanCount = 1;

    for(int chan=0; chan<curChanCount; chan++)
    {
        if(mPlotChan == -1)
            chanCycle += 1;
        else
            chanCycle = mPlotChan;
        if(chanCycle>7)
            chanCycle = chanCycle % 8;
        switch(chanCycle)
        {
        case 0:
            penColor = Qt::blue;
            break;
        case 1:
            penColor = Qt::red;
            break;
        case 2:
            penColor = Qt::green;
            break;
        case 3:
            penColor = Qt::cyan;
            break;
        case 4:
            penColor = Qt::darkCyan;
            break;
        case 5:
            penColor = Qt::magenta;
            break;
        case 6:
            penColor = Qt::gray;
            break;
        default:
            penColor = Qt::black;
            break;
        }
        dataPlot->addGraph();
        dataPlot->addGraph();
        dataPlot->graph(chan)->setPen(penColor);
    }
    dataPlot->xAxis2->setVisible(true);
    dataPlot->xAxis2->setTickLabels(false);
    dataPlot->axisRect(0)->setAutoMargins(QCP::msLeft|QCP::msBottom);
    dataPlot->axisRect(0)->setMargins(QMargins(0,2,2,0));
    dataPlot->yAxis2->setVisible(true);
    dataPlot->yAxis2->setTickLabels(false);
    dataPlot->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
    dataPlot->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
    dataPlot->xAxis->setTickLabelColor(Qt::blue);
    dataPlot->yAxis->setTickLabelColor(Qt::blue);
    dataPlot->xAxis->setAutoTickCount(3);
}

void HatDevice::plotScan(unsigned long long currentCount, long long currentIndex, int blockSize)
{
    xValues.resize(blockSize);
    double *xData = xValues.data();
    yChans.resize(mChanCount);
    for (int chan=0; chan<mChanCount; chan++)
        yChans[chan].resize(blockSize);

    int curScan, plotData, curChanCount;
    int listIndex;
    int sampleNum = 0;
    int increment = 0;
    long long totalSamples;

    totalSamples = mChanCount * ui->leNumSamples->text().toLong();

    for (int y = 0; y < blockSize; y++) {
        curScan = currentIndex + increment;
        if (!(curScan < totalSamples)) {
            currentIndex = 0;
            curScan = 0;
            increment = 0;
        }
        xData[y] = currentCount + sampleNum;
        for (int chan = 0; chan < mChanCount; chan++) {
            yChans[chan][y] = buffer[curScan + chan];
            sampleNum++;
       }
        increment +=mChanCount;
    }

    curChanCount = 1;
    if (mPlotChan == -1)
        curChanCount = mChanCount;

    for (int plotNum=0; plotNum<curChanCount; plotNum++) {
        plotData = mPlotChan;
        listIndex = plotNum % 8;
        if (mPlotChan == -1)
            plotData = plotNum;

        if (mPlotList[listIndex])
            ui->AiPlot->graph(plotNum)->setData(xValues, yChans[plotData]);
    }
    updatePlot();
}

void HatDevice::printData(unsigned long long currentCount, long long currentIndex, int blockSize)
{
    QString dataText, str, val;
    double curSample;
    int curScan, samplesToPrint, sampleLimit;
    int sampleNum = 0;
    int increment = 0;
    bool floatValue;
    long long samplePerChanel = mChanCount * ui->leNumSamples->text().toLongLong();;
    //ui->textEdit->setText(QString("Chans: %1, perChan: %2").arg(mChanCount).arg(samplePerChanel));

    floatValue = (!(mScanOptions & OPTS_NOSCALEDATA));

    ui->teShowValues->clear();
    dataText = "<style> th, td { padding-right: 10px;}</style><tr>";
    sampleLimit = mRunning? 100 : 1000 / mChanCount;
    samplesToPrint = blockSize < sampleLimit? blockSize : sampleLimit;
    for (int y = 0; y < samplesToPrint; y++) {
        curScan = currentIndex + increment;
        if (!(curScan < samplePerChanel)) {
            currentIndex = 0;
            curScan = 0;
            sampleNum = 0;
        }
        dataText.append("<td>" + str.setNum(currentCount + increment) + "</td>");
        for (int chan = 0; chan < mChanCount; chan++) {
            curSample = buffer[curScan + chan];
            if (floatValue) {
                val = QString("%1%2").arg((curSample < 0) ? "" : "+")
                        .arg(curSample, 2, 'f', 5, '0');
            } else {
                val = QString("%1").arg(curSample);
            }
            dataText.append("<td>" + val + "</td>");
        }
        dataText.append("</tr><tr>");
        sampleNum = sampleNum + 1;
        increment +=mChanCount;
    }
    dataText.append("</td></tr>");
    ui->teShowValues->setHtml(dataText);
    if (samplesToPrint < blockSize)
        ui->teShowValues->append("...");
}

void HatDevice::updatePlot()
{
    bool setTCRange = false;
    bool autoScale, bipolar;
    double rangeBuf;
    double rangeUpper, rangeLower;
    int ctlIndex;

    for (int plotNum=0; plotNum<mChanCount; plotNum++) {
        ctlIndex = plotNum % 8;
        rbPlotSel[ctlIndex]->setVisible(true);
        if (!mPlotList[ctlIndex])
            ui->AiPlot->graph(plotNum)->clearData();
    }
    for (int i = mChanCount; i<8; i++)
        rbPlotSel[i]->setVisible(false);
    autoScale = ui->rbAutoScale->isChecked();
    if(autoScale)
        ui->AiPlot->rescaleAxes();
    else {
        /*if (mRange == BIPPT078VOLTS) {
            AiConfigItem configItem = AI_CFG_CHAN_TYPE;
            unsigned int index = 0;
            long long configValue;
            UlError err = ulAIGetConfig(mDaqDeviceHandle,
                                        configItem,  index, &configValue);
            if ((err == ERR_NO_ERROR) && (configValue == 2))
                setTCRange = true;
        }*/
        setTCRange = false;
        if (setTCRange) {
            rangeBuf = 0;
            rangeUpper = 35;
            rangeLower = 15;
        } else {
            if (mScanOptions & OPTS_NOSCALEDATA) {
                long fsCount = qPow(2, mAiResolution);
                rangeBuf = fsCount / 10;
                rangeUpper = fsCount;
                rangeLower = 0;
            } else {
                bipolar = true; //mRange < 100;
                double rangeVolts = 20; //getRangeVolts(mRange);
                rangeBuf = rangeVolts / 10;
                rangeUpper = bipolar? rangeVolts / 2 : rangeVolts;
                rangeLower = bipolar? rangeUpper * -1 : 0;
            }
        }
        ui->AiPlot->xAxis->rescale();
        ui->AiPlot->yAxis->setRangeLower(rangeLower - rangeBuf);
        ui->AiPlot->yAxis->setRangeUpper(rangeUpper + rangeBuf);
    }
    ui->AiPlot->replot();
}

void HatDevice::replot()
{
    updatePlot();
}

void HatDevice::plotSelect()
{
    for (int i = 0; i<8; i++)
        mPlotList[i] = rbPlotSel[i]->isChecked();

    int plotSize = ui->leBlockSize->text().toInt();
    if (!mRunning)
        plotScan(0, 0, plotSize);
}
