#include "dioform.h"
#include "ui_dioform.h"
#include "childwindow.h"

DioForm::DioForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DioForm)
{
    ui->setupUi(this);

    hatInterface = new HatInterface();

    ui->teShowValues->setFont(QFont ("Courier", 8));
    ui->lblRateReturned->setFont(QFont ("Courier", 8));
    ui->teShowValues->setStyleSheet("QTextEdit { background-color : white; color : blue; }" );
    ui->lblStatus->setStyleSheet("QLabel { color : blue; } ");
    ui->lblInfo->setStyleSheet("QLabel { color : blue; } ");
    ui->lblRateReturned->setStyleSheet("QLabel { background-color : white; color : blue; }" );

    ui->cmbConfig->addItem("Direction");
    ui->cmbConfig->addItem("Pull config");
    ui->cmbConfig->addItem("Pull enable");
    ui->cmbConfig->addItem("Input invert");
    ui->cmbConfig->addItem("Input latch");
    ui->cmbConfig->addItem("Output type");
    ui->cmbConfig->addItem("Interrupt mask");
    ui->cmbConfig->addItem("Interrupt status");
    ui->cmbConfig->setCurrentIndex(1);
    connect(ui->cmdGo, SIGNAL(clicked(bool)), this, SLOT(onClickCmdGo()));
    createBitBoxes();
}

DioForm::~DioForm()
{
    delete ui;
}

void DioForm::updateParameters()
{
    ChildWindow *parentWindow;
    //bool showStop = false;
    //bool enableIt = false;

    parentWindow = qobject_cast<ChildWindow *>(this->parent());
    mDevName = parentWindow->devName();
    mHatID = parentWindow->devId();

    this->setWindowTitle(mFuncName + ": " + mDevName);
}

void DioForm::groupChanged(int newGroup)
{
    mCurGroup = newGroup;
}

void DioForm::setUiForGroup()
{

}

void DioForm::functionChanged(int utFunction)
{
    mUtFunction = utFunction;
    /*portList.clear();
    foreach (QCheckBox *chkPort, portCheckBoxes) {
        if (chkPort->isChecked()) {
            int portNum = chkPort->property("portNum").toInt();
            DigitalPortType portType = (DigitalPortType)portNum;
            portList.append(portType);
        }
    }*/

    //disableCheckboxInput(false);
    this->setUiForFunction();
}

void DioForm::setUiForFunction()
{
    bool scanVisible, portsVisible, stopVisible;
    bool scanParamsVisible, setNumberVisible, asyncVisible;
    bool configVisible;
    int stackIndex;
    ChildWindow *parentWindow;

    parentWindow = qobject_cast<ChildWindow *>(this->parent());
    parentWindow->setTmrEnabled(false);

    portsVisible = false;
    asyncVisible = false;
    scanVisible = false;
    scanParamsVisible = false;
    setNumberVisible = false;
    configVisible = false;
    stackIndex = 0;
    stopVisible = false;
    mPlot = false;
    QString goText = "Read";
    QString stopText = "Stop";
    QString sampToolTip = "Value read";
    QString startSample = "7";
    QString rateVal = "1000";
    QString blockText = "1000";
    //int defPort;
    switch (mUtFunction) {
    case UL_D_CONFIG_PORT:
        mFuncName = "ulDConfigPort";
        portsVisible = true;
        asyncVisible = true;
        setNumberVisible = true;
        configVisible = true;
        if (mCurGroup == FUNC_GROUP_DOUT) {
            sampToolTip = "Value to write";
            goText = "Write";
        }
        break;
    case UL_D_CONFIG_BIT:
        mFuncName = "ulDConfigBit";
        stackIndex = 1;
        asyncVisible = true;
        stopVisible = true;
        configVisible = true;
        sampToolTip = "Bit and Port to read/write (bit, port format)";
        stopText = "Write";
        setNumberVisible = true;
        /*foreach (DigitalPortType pt, validPorts) {
            setDefaultBits(pt);
        }
        defPort = (int)validPorts[0];*/
        //startSample = QString("0, %1").arg(defPort);
        break;
    case UL_D_IN:
        mFuncName = "ulDIn";
        asyncVisible = true;
        startSample = "10";
        sampToolTip = "Samples per channel";
        setNumberVisible = true;
        break;
    case UL_D_OUT:
        portsVisible = true;
        mFuncName = "ulDOut";
        goText = "Write";
        asyncVisible = true;
        setNumberVisible = true;
        break;
    case UL_D_BIT_IN:
        mFuncName = "ulDBitIn";
        asyncVisible = true;
        stackIndex = 1;
        break;
    case UL_D_BIT_OUT:
        mFuncName = "ulDBitOut";
        asyncVisible = true;
        goText = "Read";
        stackIndex = 1;
        break;
    case UL_D_INSCAN:
        mFuncName = "ulDInScan";
        asyncVisible = true;
        goText = "Go";
        sampToolTip = "Samples per channel";
        startSample = "1000";
        scanVisible = true;
        mPlot = true;
        setNumberVisible = true;
        mPlot = true;
        stackIndex = 2;
        break;
    /*
    case UL_D_OUTSCAN:
        mFuncName = "ulDOutScan";
        asyncVisible = true;
        scanVisible = true;
        setNumberVisible = true;
        goText = "Go";
        sampToolTip = "Samples per channel";
        startSample = "1000";
        mPlot = true;
        stackIndex = 2;
        break;
    */
    default:
        break;
    }
    ui->stackedWidget->setVisible(!portsVisible);
    ui->stackedWidget->setCurrentIndex(stackIndex);
    ui->fraScan->setVisible(scanVisible);
    ui->fraAsync->setVisible(asyncVisible);
    ui->cmbConfig->setVisible(configVisible);
    //ui->fraPorts->setVisible(portsVisible);
    ui->spnLowChan->setVisible(scanParamsVisible);
    ui->spnHighChan->setVisible(scanParamsVisible);
    ui->leNumSamples->setVisible(setNumberVisible);
    ui->leRate->setText(rateVal);
    ui->leNumSamples->setToolTip(sampToolTip);
    ui->leNumSamples->setText(startSample);
    ui->leBlockSize->setText(blockText);
    ui->cmdGo->setText(goText);
    ui->cmdStop->setVisible(stopVisible);
    ui->cmdStop->setText(stopText);
    //parentWindow->adjustSize();
    //updateControlDefaults(false);
    if (mPlot)
        showPlotWindow(mPlot);
    ui->cmdGo->setFocus();
    this->setWindowTitle(mFuncName + ": " + mDevName);
}

void DioForm::showPlotWindow(bool showIt)
{
    QFrame::Shape frameShape;

    if ((mUtFunction == UL_D_OUTSCAN) | (mUtFunction == UL_D_INSCAN))
        frameShape = QFrame::Box;
    mPlot = showIt;
    int curIndex = 0;
    if (showIt) {
        curIndex = 2;
        frameShape = QFrame::NoFrame;
    }
    ui->stackedWidget->setFrameShape(frameShape);
    ui->stackedWidget->setCurrentIndex(curIndex);
}

void DioForm::initDeviceParams()
{

}

void DioForm::configureInputs()
{

}

void DioForm::configureOutputs()
{

}

void DioForm::onClickCmdGo()
{
    runSelectedFunction();
}

void DioForm::bitToggled(int bitNumber)
{
    //int curBit;

    mUtFunction = UL_D_CONFIG_PORT;
    bool setValue = chkBit[bitNumber]->isChecked();
    unsigned int bitVal = 0;
    //DigitalDirection direction = DD_INPUT;
    if (setValue) {
        bitVal = 1;
        //direction = DD_OUTPUT;
    }
    //mapGridToPortBit(bitNumber, portType, curBit);

    switch (mUtFunction) {
    case UL_D_CONFIG_BIT:
        //runDConfigBit(portType, curBit, direction);
        break;
    case UL_D_IN:
        break;
    case UL_D_OUT:
        runDOutFunc();
        break;
    case UL_D_BIT_IN:
        break;
    case UL_D_BIT_OUT:
        runDBitOutFunc(bitNumber, bitVal);
        break;
    default:
        ui->lblInfo->setText(QString("Bit %1 clicked as bit value %2")
                               //.arg(portType)
                               .arg(bitNumber)
                               .arg(bitVal));
        break;
    }
}

void DioForm::runSelectedFunction()
{
    ChildWindow *parentWindow;
    QFont goFont = ui->cmdGo->font();
    bool makeBold, tmrIsEnabled, tmrIsRunning;
    //bool isBipolar;
    bool showStop = false;
    //int chanScale;
    //double defaultRange, offset;

    parentWindow = qobject_cast<ChildWindow *>(this->parent());

    /*mChanCount = portList.count();
    if (mUtFunction == UL_D_OUTSCAN) {
        if (mChanCount) {
            for (int i = 0; i < mChanCount; i++) {
                int numWaves = mWaves.count() - 1;
                if (i > numWaves) {
                    //if data hasn't been defined set default data
                    chanScale = DMgr::counts;
                    defaultRange = (pow(2, mDioResolution)) - 2;
                    offset = defaultRange / 2;
                    isBipolar = false;
                    mWaves.insert(i, DMgr::sineWave);
                    mCycles.insert(i, 1);
                    mAmplitude.insert(i, defaultRange);
                    mOffset.insert(i, offset);
                    mDataScale.insert(i, chanScale);
                    mBipolar.insert(i, isBipolar);
                }
            }
        }
    }*/

    switch (mUtFunction) {
    case UL_D_CONFIG_PORT:
        runDOutFunc();
        break;
    case UL_D_CONFIG_BIT:
        showStop = true;
        //readSingleBit();
        break;
    case UL_D_IN:
        runDInFunc();
        break;
    case UL_D_OUT:
        runDOutFunc();
        break;
    case UL_D_BIT_IN:
        //disableCheckboxInput(true);
        //runDBitInFunc();
        break;
    case UL_D_BIT_OUT:
        //runDBitInFunc();
        break;
    /*
    case UL_D_INSCAN:
        showStop = true;
        mTriggerType = parentWindow->triggerType();
        if (!mTriggerType == TRIG_NONE) {
            mTrigChannel = parentWindow->trigChannel();
            mTrigLevel = parentWindow->trigLevel();
            mTrigVariance = parentWindow->trigVariance();
            mRetrigCount = parentWindow->retrigCount();
            qApp->processEvents();
            runSetTriggerFunc();
        }
        runDInScanFunc();
        break;
    case UL_D_OUTSCAN:
        showStop = true;
        getDataValues();
        mTriggerType = parentWindow->triggerType();
        if (!mTriggerType == TRIG_NONE) {
            mTrigChannel = parentWindow->trigChannel();
            mTrigLevel = parentWindow->trigLevel();
            mTrigVariance = parentWindow->trigVariance();
            mRetrigCount = parentWindow->retrigCount();
            qApp->processEvents();
            runSetTriggerFunc();
        }
        runDOutScanFunc();
        break;
    */
    default:
        break;
    }

    tmrIsEnabled = parentWindow->tmrEnabled();
    tmrIsRunning = parentWindow->tmrRunning();
    if ((!tmrIsEnabled) & (!showStop)) {
        if (tmrIsRunning)
            parentWindow->setTmrRunning(false);
        showStop = false;
        mUseTimer = false;
        goFont.setBold(false);
        ui->cmdGo->setFont(goFont);
    } else {
        if (mUtFunction == UL_D_CONFIG_BIT) {
            showStop = true;
        } else if (mUseTimer) {
            if (!tmrIsRunning) {
                parentWindow->setTmrRunning(true);
                ui->cmdStop->setEnabled(true);
            }
            showStop = true;
            makeBold = !ui->cmdGo->font().bold();
            goFont.setBold(makeBold);
            ui->cmdGo->setFont(goFont);
        } else {
            if (tmrIsRunning) {
                parentWindow->setTmrRunning(false);
                ui->cmdStop->setEnabled(false);
            }
            goFont.setBold(false);
            ui->cmdGo->setFont(goFont);
        }
    }
    ui->cmdStop->setVisible(tmrIsEnabled | showStop);
}

void DioForm::runDInFunc()
{
}

void DioForm::runDBitInFunc(uint8_t value, uint8_t bit)
{
    mResponse = hatInterface->dioInputRead(mHatID, mAddress, bit, value);
}

void DioForm::runDOutFunc()
{

}

void DioForm::runDBitOutFunc(uint8_t value, uint8_t bit)
{
    mResponse = hatInterface->dioOutputWrite(mHatID, mAddress, bit, value);
}

void DioForm::onClickCmdStop()
{

}

void DioForm::stopGoTimer()
{

}

void DioForm::showQueueConfig()
{

}

void DioForm::createBitBoxes()
{
    //int offset, portIndex, bitIndex;
    int pos;
    QString portBit; //, str;
    QSignalMapper *mapBits = new QSignalMapper(ui->stackedWidget->widget(1));
    QGridLayout *bitLayout = new QGridLayout(ui->stackedWidget->widget(1));
    //QColor blue( 100, 180, 255 );

    connect(mapBits, SIGNAL(mapped(int)), this, SLOT(bitToggled(int)));
    bitLayout->setSpacing(1);
    bitLayout->setMargin(4);
    bitLayout->setSizeConstraint(QLayout::SetFixedSize);
    bitLayout->setColumnMinimumWidth(3, 40);

    //spacing between auxport and firsport set by col width
    pos = 0;
    /*offset = 0;
    portIndex = 0;
    bitIndex = 0;*/

    for (int i=0; i<numBitCheckboxes; i++) {
        chkBit[i] = new QCheckBox(ui->stackedWidget->widget(1));
        chkBit[i]->setTristate(true);
        chkBit[i]->setCheckState(Qt::PartiallyChecked);
        mapBits->setMapping(chkBit[i], i);
        connect(chkBit[i], SIGNAL(clicked(bool)), mapBits, SLOT(map()));
        chkBit[i]->setMaximumHeight(16);
        chkBit[i]->setMaximumWidth(20);
        chkBit[i]->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        /*if (i < 16)
            portBit = QString("Aux1 bit %1").arg(i);
        else if (i < 32)
            portBit = QString("Aux2 bit %1").arg(i - 16);
        else {
            if  (i - 32) {
                if (!((i - 32 - offset) % 20)) {
                    pos += 4;
                    bitIndex = 0;
                    portIndex += 1;
                } else if (!((i - 32) % 24)) {
                    pos += 4;
                    offset += 4;
                    bitIndex = 0;
                    portIndex += 1;
                }
            }
            if ((i > 32) && (bitIndex > 0))
                if (!(i % 8)) {
                    bitIndex = 0;
                    portIndex += 1;
                }
            portBit = portNames[portIndex] + str.setNum(bitIndex);
            bitIndex += 1;
        }
        */
        chkBit[i]->setToolTip(portBit);
        bitLayout->addWidget(chkBit[i], pos % 8, pos / 8);
        pos++;
    }
    ui->stackedWidget->widget(1)->setLayout(bitLayout);
}
