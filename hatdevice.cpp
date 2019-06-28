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

    tmrGoTimer = new QTimer(this);
    tmrBGResultRead = new QTimer(this);
    mUseGetStatus = true;
    mStopOnStart = false;
    functionGroup = new QActionGroup(this);
    buffer = NULL;
    data = NULL;

    mAddress = -1;
    mNumHats = 0;
    mScanOptions = OPTS_DEFAULT;
    mTriggerType = TRIG_RISING_EDGE;
    mTriggerSource = SOURCE_LOCAL;
    mAiResolution = 12;
    mCurFunction = UL_AIN;
    mTriggered = false;
    mBackgroundScan = true;
    mQueueEnabled = false;
    mOneSampPerForTotalSamps = false;
    mTotalRead = 0;

    ui->teShowValues->setFont(QFont ("Courier", 8));
    ui->teShowValues->setStyleSheet("QTextEdit { background-color : white; color : blue; }" );
    ui->lblRateReturned->setFont(QFont ("Courier", 8));
    ui->lblRateReturned->setStyleSheet("QLabel { background-color : white; color : blue; }" );
    ui->lblBufferSize->setFont(QFont ("Courier", 8));
    ui->lblBufferSize->setStyleSheet("QLabel { background-color : white; color : grey; }" );
    ui->lblStatus->setStyleSheet("QLabel { color : blue; }" );
    ui->lblInfo->setStyleSheet("QLabel { color : blue; }" );

    connect(tmrGoTimer, SIGNAL(timeout()), this, SLOT(checkStatus()));
    //connect(tmrBGResultRead, SIGNAL(timeout()), this, SLOT(runBackgrndResult()));
    connect(ui->cmdGo, SIGNAL(clicked(bool)), this, SLOT(goCmdClicked()));
    connect(ui->cmdStop, SIGNAL(clicked(bool)), this, SLOT(stopCmdClicked()));

    connect(ui->AiPlot->xAxis, SIGNAL(rangeChanged(QCPRange)),
            ui->AiPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->AiPlot->yAxis, SIGNAL(rangeChanged(QCPRange)),
            ui->AiPlot->yAxis2, SLOT(setRange(QCPRange)));
    connect(ui->rbAutoScale, SIGNAL(clicked(bool)), this, SLOT(replot()));
    connect(ui->rbFullScale, SIGNAL(clicked(bool)), this, SLOT(replot()));
    connect(ui->cmdSetTcConf, SIGNAL(clicked(bool)), this, SLOT(writeStoredTcValues()));
    connect(ui->cmdReadTcConf, SIGNAL(clicked(bool)), this, SLOT(updateTcValues()));
    connect(ui->cmdSetInterval, SIGNAL(clicked(bool)), this, SLOT(setInterval()));

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
        stopCmdClicked();
    }
    if (keyCode == Qt::Key_F6)
        updateData();
    if (keyCode == Qt::Key_F12)
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
    uint32_t allOptions;

    parentWindow = qobject_cast<ChildWindow *>(this->parent());
    mDevName = parentWindow->devName();
    mAddress = parentWindow->devAddress();
    mHatID = parentWindow->devId();
    allOptions = parentWindow->scanOptions();
    mScanOptions = allOptions & 0xFFF;
    mBackgroundScan = ((allOptions & 0x1000) != 0);
    mTriggerType = parentWindow->triggerType();

    mUseTimer = parentWindow->tmrEnabled();
    mStopOnStart = parentWindow->tmrStopOnStart();
    mGoTmrIsRunning = parentWindow->tmrRunning();
    mTmrInterval = parentWindow->tmrInterval();
    mOneSampPerForTotalSamps = parentWindow->tmrSampPerInterval();
    //mTInPrefs = parentWindow->tInPrefs();

    mOptNames = getOptionNames(allOptions);
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
    bool cjcCheckVisible, intervalSetVisible;
    bool scanConfigVisible, scanParamsVisible;
    bool setTcCmdVisible;
    QString blockToolTip;

    //mChanList.clear();
    //mRangeList.clear();
    mPlot = false;
    scanVisible = false;
    voltCheckVisible = false;
    cjcCheckVisible = false;
    scanConfigVisible = false;
    setTcCmdVisible = false;
    intervalSetVisible = false;
    scanParamsVisible = false;
    blockToolTip = "Block size";

    switch (mCurFunction) {
    case UL_AIN:
        mFuncName = "ulAIn";
        ui->leNumSamples->setText("10");
        break;
    case UL_TIN:
        mFuncName = "ulTIn";
        ui->leNumSamples->setText("10");
        voltCheckVisible = true;
        cjcCheckVisible = true;
        scanConfigVisible = true;
        setTcCmdVisible = true;
        scanParamsVisible = true;
        intervalSetVisible = true;
        blockToolTip = "Interval";
        checkCurTcConfig(false);
        break;
    case UL_AINSCAN:
        mFuncName = "ulAInScan";
        scanConfigVisible = true;
        scanVisible = true;
        scanParamsVisible = true;
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
    ui->fraCJC->setVisible(cjcCheckVisible);
    ui->fraChan->setVisible(cjcCheckVisible);
    ui->fraChanScan->setVisible(!cjcCheckVisible);
    ui->fraScanConf->setVisible(scanConfigVisible);
    ui->fraSetTc->setVisible(setTcCmdVisible);
    ui->fraScanParams->setVisible(scanParamsVisible);
    ui->cmdSetInterval->setVisible(intervalSetVisible);
    ui->leBlockSize->setToolTip(blockToolTip);
    //ui->cmdStop->setEnabled(false);
    showPlotWindow(mPlot);
    this->setWindowTitle(mFuncName + ": " + mDevName);
}

void HatDevice::functionChanged(int utFunction)
{
    mCurFunction = utFunction;
    this->setUiForFunction();
}

void HatDevice::updateTcValues()
{
    checkCurTcConfig(true);
}

void HatDevice::checkCurTcConfig(bool saveToChild)
{
    QSettings windowSettings("Measurement Computing", "Qt Hat Test Linux");
    QVariant storedTInPrefs;
    ChildWindow *parentWindow;
    parentWindow = qobject_cast<ChildWindow *>(this->parent());
    QString typePrefs, serNum, typeName;
    uint8_t chan, interval;
    int numChans;
    uint8_t tcType;

    mResponse = hatInterface->getSerialNumber(mHatID, mAddress, serNum);
    ui->lblInfo->setText(hatInterface->getStatus());

    if(mResponse == RESULT_SUCCESS) {
        mSerNum = QString("%1").arg(serNum);
        parentWindow->setSerNum(mSerNum);
    }
    mResponse = hatInterface->readInterval(mHatID, mAddress, interval);
    if(mResponse == RESULT_SUCCESS)
        ui->leBlockSize->setText(QString("%1").arg(interval));

    windowSettings.beginGroup(mSerNum);
    storedTInPrefs = windowSettings.value("TempPrefs", "0,0,0,0");
    mTInPrefs = storedTInPrefs.toString();
    windowSettings.endGroup();
    if(mTInPrefs == "")
        mTInPrefs = "255,255,255,255";

    typePrefs = "";
    numChans = hatInterface->getNumAInChans(mHatID);
    for(chan = 0; chan < numChans; chan++) {
        mResponse = hatInterface->readTcTypes(mHatID, mAddress, chan, tcType);
        ui->lblInfo->setText(hatInterface->getStatus());
        if(mResponse != RESULT_SUCCESS)
            break;
        else {
            typePrefs += QString("%1,").arg(tcType);
            typeName = getTcTypeName(tcType);
            switch (chan) {
            case 0:
                ui->chkChan0->setChecked(tcType != TC_DISABLED);
                ui->chkChan0->setToolTip(typeName);
                break;
            case 1:
                ui->chkChan1->setChecked(tcType != TC_DISABLED);
                ui->chkChan1->setToolTip(typeName);
                break;
            case 2:
                ui->chkChan2->setChecked(tcType != TC_DISABLED);
                ui->chkChan2->setToolTip(typeName);
                break;
            case 3:
                ui->chkChan3->setChecked(tcType != TC_DISABLED);
                ui->chkChan3->setToolTip(typeName);
                break;
            default:
                break;
            }
        }
    }
    typePrefs.chop(1);
    //mTInPrefs = typePrefs;
    //parentWindow->setTInPrefs(mTInPrefs);
    //ui->cmdSetTcConf->setEnabled(mTInPrefs != typePrefs);
    ui->lblStatus->setText(mSerNum + " Type prefs = " + typePrefs + " stored prefs = " + mTInPrefs);
    if(saveToChild) {
        parentWindow->setTInPrefs(typePrefs);
        ui->lblStatus->setText(mSerNum + " Type prefs = " + typePrefs + " Wrote " + typePrefs);
        mTInPrefs = typePrefs;
    }
}

void HatDevice::setInterval()
{
    uint8_t interval;

    interval = ui->leBlockSize->text().toUInt();
    mResponse = hatInterface->writeInterval(mHatID, mAddress, interval);
    ui->lblStatus->setText(hatInterface->getStatus());
}

void HatDevice::writeStoredTcValues()
{
    uint8_t tcType;
    QString tInPrefs;
    QVector<uint8_t> typeList;
    bool writeFail;

    ui->teShowValues->clear();
    tInPrefs = "";
    writeFail = false;

    typeList = getTcTypesFromString(mTInPrefs);
    for (int curChan = 0; curChan < typeList.length(); curChan++) {
        tcType = typeList.at(curChan);
        mResponse = hatInterface->writeTcType(mHatID, mAddress, curChan, tcType);
        if (mResponse != RESULT_SUCCESS) {
            writeFail = true;
            break;
        }
    }
    checkCurTcConfig(!writeFail);
}

void HatDevice::goCmdClicked()
{
    ChildWindow *parentWindow;
    parentWindow = qobject_cast<ChildWindow *>(this->parent());
    bool tmrIsEnabled;

    mTotalRead = 0;
    tmrIsEnabled = parentWindow->tmrEnabled();
    mUseTimer = tmrIsEnabled;
    mTimerConfigured = tmrIsEnabled;
    runSelectedFunction();
}

void HatDevice::stopCmdClicked()
{
    stopScan();
    mUseTimer = false;
}

void HatDevice::runSelectedFunction()
{
    ChildWindow *parentWindow;
    QFont goFont = ui->cmdGo->font();
    bool makeBold, tmrIsEnabled, tmrIsRunning;

    parentWindow = qobject_cast<ChildWindow *>(this->parent());
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
        if(mHatID == HAT_ID_MCC_118)
            runAInScanFunc();
        else
            runAInScan172Func();
        break;
    case UL_TIN:
        runTinFunction();
        break;
    default:
        break;
    }

    tmrIsEnabled = parentWindow->tmrEnabled();
    tmrIsRunning = parentWindow->tmrRunning();

    if (!tmrIsEnabled) {
        if (tmrIsRunning)
            parentWindow->setTmrRunning(false);
        mUseTimer = false;
        goFont.setBold(false);
        ui->cmdGo->setFont(goFont);
    } else {
        if (mUseTimer) {
            if (!tmrIsRunning) {
                parentWindow->setTmrRunning(true);
            }
            makeBold = !ui->cmdGo->font().bold();
            goFont.setBold(makeBold);
            ui->cmdGo->setFont(goFont);
        } else {
            if (tmrIsRunning) {
                parentWindow->setTmrRunning(false);
            }
            goFont.setBold(false);
            ui->cmdGo->setFont(goFont);
        }
    }
}

void HatDevice::runSetTriggerFunc()
{
    QString trigString;

    mResponse = hatInterface->setTrigger(mHatID, mAddress, mTriggerSource, mTriggerType);
    ui->lblStatus->setText(hatInterface->getStatus());

    trigString = getTrigText(mTriggerType);
    ui->lblInfo->setText("Trigger type: " + trigString);

}

void HatDevice::runAinFunction()
{
    uint8_t aInChan, aInLastChan;
    uint32_t samplesToRead;
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

    samplesToRead = mSamplesPerChan;
    curIndex = 0;
    if(mOneSampPerForTotalSamps) {
        samplesToRead = 1;
        curIndex = mTotalRead * mChanCount;
    }

    if((!mOneSampPerForTotalSamps) | (mTotalRead == 0)) {
        if (buffer) {
            delete[] buffer;
            buffer = NULL;
        }

        long long bufSize = mChanCount * mSamplesPerChan;
        mBufSize = bufSize;
        buffer = new double[bufSize];
        memset(buffer, 0.00000001, mBufSize * sizeof(*buffer));
    }

    uint8_t curChan;
    curIndex = 0;
    mRunning = true;
    for (uint32_t sampleNum = 0; sampleNum < samplesToRead; sampleNum++) {
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
        mTotalRead += 1;
    }
    if(mOneSampPerForTotalSamps) {
        QString timerRate = ".";
        if(mUseTimer)
            timerRate = QString(" at %1 second rate.").arg(mTmrInterval / 1000);
        ui->lblStatus->setText(QString("%1 samples read%2")
                               .arg(mTotalRead).arg(timerRate));
    }

    mRunning = false;
    if(mPlot)
        plotScan(0, 0, mTotalRead);
    else {
        mTextIndex = 0;
        printData(0, 0, mTotalRead);
    }
    if(mTotalRead == mSamplesPerChan) {
        mUseTimer = false;
    }
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
        mResponse = hatInterface->getBufferSize(mHatID, mAddress, bufferSize);
        ui->lblBufferSize->setText(QString("%1").arg(bufferSize));
        if(mBackgroundScan) {
            mStatusTimerEnabled = true;
            tmrGoTimer->start(500);
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
                else {
                    mTextIndex = 0;
                    printData(0, 0, sampsReadPerChan);
                }
            }
            stopScan();
        }
    }
}

void HatDevice::runAInScan172Func()
{
    int lowChan, highChan;
    uint8_t chanMask;
    int32_t sampsToRead;
    uint32_t bufferSize;
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime, statString;

    if (mHatID != HAT_ID_MCC_172) {
        //only compatible with 172
        ui->lblStatus->setText("This function is not supported for this device");
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
    mResponse = hatInterface->ainClockConfigWrite(mHatID, mAddress, SOURCE_LOCAL, rate);
    ui->lblInfo->setText(hatInterface->getStatus());
    if(mResponse != RESULT_SUCCESS)
        return;

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

#ifdef HAT_05
    uint8_t source;
    uint8_t value;
    mResponse = hatInterface->ainClockConfigRead(mHatID, mAddress, source, mRateReturned, value);
    ui->lblInfo->setText(hatInterface->getStatus());
    if(mResponse != RESULT_SUCCESS)
        return;
    /*mResponse = hatInterface->getBufferSize(mHatID, mAddress, bufferSize);
    ui->lblInfo->setText(hatInterface->getStatus());
    if(mResponse != RESULT_SUCCESS)
        return;*/
    nameOfFunc = "172: AInScanStart";
    funcArgs = "(mAddress, chanMask, mSamplesPerChan, mScanOptions)\n";
    sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
    mResponse = mcc172_a_in_scan_start(mAddress, chanMask, mSamplesPerChan, mScanOptions);
    argVals = QStringLiteral("(%1, %2, %3, %4)")
            .arg(mAddress)
            .arg(chanMask)
            .arg(mSamplesPerChan)
            .arg(mScanOptions);
    ui->lblInfo->setText(nameOfFunc + argVals + "   " +
                         mOptNames + QString("  [Error = %1]").arg(mResponse));
#endif

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    hatInterface->reportResult(mResponse, sStartTime + funcStr);
    if (mResponse!=RESULT_SUCCESS) {
        mStatusTimerEnabled = false;
    } else {
        mRunning = true;
#ifdef HAT_05
/*        uint8_t source;
        uint8_t value;
        mResponse = hatInterface->ainClockConfigRead(mHatID, mAddress, source, mRateReturned, value);
        ui->lblInfo->setText(hatInterface->getStatus());
        if(mResponse != RESULT_SUCCESS)
            return;*/
        mResponse = hatInterface->getBufferSize(mHatID, mAddress, bufferSize);
        ui->lblInfo->setText(hatInterface->getStatus());
        if(mResponse != RESULT_SUCCESS)
            return;
#endif
        ui->lblRateReturned->setText(QString("%1").arg(mRateReturned, 1, 'f', 4, '0'));
        ui->lblBufferSize->setText(QString("%1").arg(bufferSize));
        if(mBackgroundScan) {
            mStatusTimerEnabled = true;
            tmrGoTimer->start(500);
        } else {
            uint16_t status;
            double timeout;
            uint32_t sampsReadPerChan;
            timeout = ui->leTimeout->text().toDouble();
            nameOfFunc = "172: AInScanRead";
            funcArgs = "(mAddress, status, mSamplesToRead, timo, buffer, bufSize, numRead)\n";
            sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
            do {
                mResponse = mcc172_a_in_scan_read(mAddress, &status, 0, timeout, NULL, 0, NULL);
                if(mResponse != RESULT_SUCCESS)
                    status = 0;
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
            mResponse = mcc172_a_in_scan_read(mAddress, &status, sampsToRead, timeout, buffer, mBufSize, &sampsReadPerChan);
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
                else {
                    mTextIndex = 0;
                    printData(0, 0, sampsReadPerChan);
                }
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
        switch (mHatID) {
        case HAT_ID_MCC_118:
            nameOfFunc = "118: AInScanRead";
            sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
            mResponse = mcc118_a_in_scan_read(mAddress, &status, mBlockSize,
                timeout, buffer, mBufSize, &samplesPerChanRead);
            break;
#ifdef HAT_05
        case HAT_ID_MCC_172:
            nameOfFunc = "172: AInScanRead";
            mResponse = mcc172_a_in_scan_read(mAddress, &status, mBlockSize,
                timeout, buffer, mBufSize, &samplesPerChanRead);
            break;
#endif
        default:
            mResponse = RESULT_INVALID_DEVICE;
            break;
        }
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
        else {
            mTextIndex = 0;
            printData(0, 0, samplesPerChanRead);
        }
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
    QFont goFont = ui->cmdGo->font();

    tmrGoTimer->stop();
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

    if (!buffer)
        return;
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
        switch (mHatID) {
        case HAT_ID_MCC_118:
            nameOfFunc = "118: AInScanRead";
            sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
            mResponse = mcc118_a_in_scan_read(mAddress, &status, mBlockSize,
                timeout, buffer, mBufSize, &samplesPerChanRead);
            break;
#ifdef HAT_05
        case HAT_ID_MCC_172:
            nameOfFunc = "172: AInScanRead";
            sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
            mResponse = mcc172_a_in_scan_read(mAddress, &status, mBlockSize,
                timeout, buffer, mBufSize, &samplesPerChanRead);
            break;
#endif
        default:
            mResponse = RESULT_INVALID_DEVICE;
            break;
        }
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
    if(buffer) {
        if(mPlot) {
            setupPlot(ui->AiPlot, mChanCount);
            plotScan(0, 0, mTotalRead);
        } else {
            mTextIndex = 0;
            printData(0, 0, mTotalRead);
        }
    }
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
    int curScan, samplesToPrint, sampleLimit, totalSamps;
    //int sampleNum = 0;
    int increment = 0;
    bool floatValue;
    long long samplePerChanel = mChanCount * ui->leNumSamples->text().toLongLong();;
    //ui->textEdit->setText(QString("Chans: %1, perChan: %2").arg(mChanCount).arg(samplePerChanel));

    (void)currentCount;
    if(!buffer)
        return;
    floatValue = (!(mScanOptions & OPTS_NOSCALEDATA));

    totalSamps = mSamplesPerChan;
    ui->teShowValues->clear();
    dataText = "<style> th, td { padding-right: 10px;}</style><tr>";
    sampleLimit = mRunning? 100 : 1000 / mChanCount;
    samplesToPrint = blockSize < sampleLimit? blockSize : sampleLimit;
    if ((samplesToPrint + mTextIndex) > (totalSamps))
        samplesToPrint = (totalSamps) - mTextIndex;
    for (int y = 0; y < samplesToPrint; y++) {
        //int increment = 0;
        curScan = currentIndex + increment;
        if (!(curScan < samplePerChanel)) {
            currentIndex = 0;
            curScan = 0;
            //sampleNum = 0;
        }
        dataText.append("<td>" + str.setNum(increment) + "</td>");
        for (int chan = 0; chan < mChanCount; chan++) {
            curSample = buffer[increment + chan];
            if (floatValue) {
                val = QString("%1%2").arg((curSample < 0) ? "" : "+")
                        .arg(curSample, 2, 'f', 5, '0');
            } else {
                val = QString("%1").arg(curSample);
            }
            dataText.append("<td>" + val + "</td>");
        }
        dataText.append("</tr><tr>");
        //sampleNum = sampleNum + 1;
        increment += mChanCount;
        mTextIndex = increment;
    }
    dataText.append("</td></tr>");
    ui->teShowValues->setHtml(dataText);
    if (samplesToPrint < totalSamps)
        ui->teShowValues->append("... (F6)");
}

void HatDevice::updateData()
{
    QString dataText, str, val;
    bool floatValue;

    floatValue = false;
    int increment = mTextIndex;
    int samplesToPrint, sampleLimit, blockSize;
    double curSample;

    //blockSize = 1000;
    blockSize = mSamplesPerChan;
    if (!buffer)
        return;
    floatValue = (!(mScanOptions & OPTS_NOSCALEDATA));

    //print only 1000
    sampleLimit = mRunning? 100 : 1000 / mChanCount;
    samplesToPrint = blockSize < sampleLimit? blockSize : sampleLimit;
    //samplesToPrint = blockSize < 1000? mSamplesPerChan : 1000;
    if (((samplesToPrint * mChanCount) + mTextIndex) > (blockSize))
        samplesToPrint = (blockSize) - (mTextIndex / mChanCount);
    ui->teShowValues->clear();
    dataText = "<style> th, td { padding-right: 10px;}</style><tr>";
    for (int y = 0; y < samplesToPrint; y++) {
        dataText.append("<td>" + str.setNum(increment) + "</td>");
        for (int chan = 0; chan < mChanCount; chan++) {
            curSample = buffer[increment + chan];
            if (floatValue)
                val = QString("%1%2").arg((curSample < 0) ? "" : "+")
                        .arg(curSample, 2, 'f', 5, '0');
            else
                val = QString("%1").arg(curSample);
            dataText.append("<td>" + val + "</td>");
        }
        dataText.append("</tr><tr>");
        increment += mChanCount;
        mTextIndex = increment;
    }
    dataText.append("</td></tr>");
    ui->teShowValues->setHtml(dataText);
    if (mTextIndex >= (blockSize * mChanCount))
        mTextIndex = 0;
    else
        ui->teShowValues->append("... (F6)");
    ui->lblInfo->setText(QString("%1, %2, %3").arg(mTextIndex).arg(blockSize).arg(samplesToPrint));
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
    setTCRange = (mCurFunction == UL_TIN);
    if(autoScale) {
        ui->AiPlot->rescaleAxes();
        double center = ui->AiPlot->yAxis->range().center();
        ui->AiPlot->yAxis->scaleRange(1.2, center);
    } else {
        if (setTCRange) {
            rangeBuf = 0;
            rangeUpper = 50;
            rangeLower = 10;
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
    if(mOneSampPerForTotalSamps)
        plotSize = mTotalRead;
    if (!mRunning)
        plotScan(0, 0, plotSize);
}

void HatDevice::runTinFunction()
{
    uint8_t cjcChan, chanMask;
    uint32_t samplesToRead;
    int curIndex, addChan, chan;
    double data;
    bool showVolts;
    uint8_t cjcChans[3];

    data = 0.0;
    addChan = 0;
    chan = 0;
    chanMask = 0;
    uint8_t curChan;
    showVolts = ui->chkVolts->isChecked();

    /*if(!mQueueEnabled) {
        mChanList.clear();
        mChanCount = 0;
        aInChan = ui->spnLowChan->value();
        aInLastChan = ui->spnHighChan->value();
        mChanCount = (aInLastChan - aInChan) + 1;
        for(int chan = 0; chan < mChanCount; chan++)
            mChanList[chan] = aInChan + chan;
        mChanCount += addChan;
    }*/

    if(!mQueueEnabled) {
        mChanList.clear();
        mChanCount = 0;
        chanMask = 0;
        for(curChan = 0; curChan < 4; curChan++) {
            cjcChans[curChan] = 8;
            switch (curChan) {
            case 0:
                if(ui->chkChan0->isChecked()) {
                    mChanList[chan] = curChan;
                    chan++;
                    mChanCount++;
                    chanMask = chanMask | 0x01 << 0;
                }
                if(ui->chkCJC_0->isChecked()) { //  && !mBackgroundScan
                    cjcChans[addChan] = 0;
                    addChan++;
                    mChanCount++;
                    //chanMask = chanMask | 0x01 << 4;
                }
                break;
            case 1:
                if(ui->chkChan1->isChecked()) {
                    mChanList[chan] = curChan;
                    chan++;
                    mChanCount++;
                    chanMask = chanMask | 0x01 << 1;
                }
                if(ui->chkCJC_1->isChecked()) { //  && !mBackgroundScan
                    cjcChans[addChan] = 1;
                    addChan++;
                    mChanCount++;
                    //chanMask = chanMask | 0x01 << 5;
                }
                break;
            case 2:
                if(ui->chkChan2->isChecked()) {
                    mChanList[chan] = curChan;
                    chan++;
                    mChanCount++;
                    chanMask = chanMask | 0x01 << 2;
                }
                if(ui->chkCJC_2->isChecked()) { //  && !mBackgroundScan
                    cjcChans[addChan] = 3;
                    addChan++;
                    mChanCount++;
                    //chanMask = chanMask | 0x01 << 6;
                }
                break;
            case 3:
                if(ui->chkChan3->isChecked()) {
                    mChanList[chan] = curChan;
                    chan++;
                    mChanCount++;
                    chanMask = chanMask | 0x01 << 3;
                }
            default:
                break;
            }
        }
    }
    if(mChanCount < 1) mChanCount = 1;

    //if queue is enabled, mChanCount is set in setupQueue
    mSamplesPerChan = ui->leNumSamples->text().toLong();
    if(mPlot)
        setupPlot(ui->AiPlot, mChanCount);

    samplesToRead = mSamplesPerChan;
    curIndex = 0;
    if(mOneSampPerForTotalSamps) {
        samplesToRead = 1;
        curIndex = mTotalRead * mChanCount;
    }

    if((!mOneSampPerForTotalSamps) | (mTotalRead == 0)) {
        if (buffer) {
            delete[] buffer;
            buffer = NULL;
        }

        long long bufSize = (mChanCount) * mSamplesPerChan;
        mBufSize = bufSize;
        buffer = new double[bufSize];
        memset(buffer, 0.00000001, mBufSize * sizeof(*buffer));
    }

    if(false) {         // !showVolts & mBackgroundScan
        for (uint32_t sampleNum = 0; sampleNum < samplesToRead; sampleNum++) {
            mResponse = hatInterface->tInReadBackground(mHatID, mAddress, chanMask);
            //int sampsRead = mTotalRead;
            if(mResponse == RESULT_SUCCESS) {
                //tmrBGResultRead->start(200);
                mRunning = true;
                do {
                    //runBackgrndResult();
                    delay(200);
                    //QApplication::processEvents(QEventLoop::AllEvents, 100);
                }
                while (mRunning);
            }
        }
    } else {
        mRunning = true;
        for (uint32_t sampleNum = 0; sampleNum < samplesToRead; sampleNum++) {
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
            for(curChan = 0; curChan < 3; curChan++) {
                cjcChan = cjcChans[curChan];
                if(cjcChan != 8) {
                    mResponse = hatInterface->boardTemp(mHatID, mAddress, cjcChan, data);
                    ui->lblInfo->setText(hatInterface->getStatus());
                    if(mResponse == RESULT_SUCCESS) {
                        buffer[curIndex] = data;
                        curIndex++;
                    } else {
                        return;
                    }
                }
            }
            mTotalRead += 1;
        }
        if(mOneSampPerForTotalSamps) {
            QString timerRate = ".";
            if(mTimerConfigured)
                timerRate = QString(" at %1 second rate.").arg(mTmrInterval / 1000);
            ui->lblStatus->setText(QString("%1 samples read%2")
                                   .arg(mTotalRead).arg(timerRate));
        }

        mRunning = false;
        if(mPlot)
            plotScan(0, 0, mTotalRead);
        else {
            mTextIndex = 0;
            printData(0, 0, mTotalRead);
        }
        if(mTotalRead == mSamplesPerChan) {
            mUseTimer = false;
        }
    }
}

