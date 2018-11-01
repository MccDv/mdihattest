#include "dioform.h"
#include "ui_dioform.h"
#include "childwindow.h"

DioForm::DioForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DioForm)
{
    ui->setupUi(this);

    hatInterface = new HatInterface();
    tmrGoTimer = new QTimer(this);
    mHistListSize = 50;

    ui->teShowValues->setFont(QFont ("Courier", 8));
    ui->lblRateReturned->setFont(QFont ("Courier", 8));
    ui->teShowValues->setStyleSheet("QTextEdit { background-color : white; color : blue; }" );
    ui->lblStatus->setStyleSheet("QLabel { color : blue; } ");
    ui->lblInfo->setStyleSheet("QLabel { color : blue; } ");
    ui->lblRateReturned->setStyleSheet("QLabel { background-color : white; color : blue; }" );

    ui->cmbConfig->addItem("Direction", DIO_DIRECTION);
    ui->cmbConfig->addItem("Pull config", DIO_PULL_CONFIG);
    ui->cmbConfig->addItem("Pull enable", DIO_PULL_ENABLE);
    ui->cmbConfig->addItem("Input invert", DIO_INPUT_INVERT);
    ui->cmbConfig->addItem("Input latch", DIO_INPUT_LATCH);
    ui->cmbConfig->addItem("Output type", DIO_OUTPUT_TYPE);
    ui->cmbConfig->addItem("Interrupt mask", DIO_INT_MASK);
    ui->cmbConfig->setCurrentIndex(0);
    mConfigItem = DIO_DIRECTION;

    connect(ui->cmdGo, SIGNAL(clicked(bool)), this, SLOT(onClickCmdGo()));
    connect(ui->cmdConfigIn, SIGNAL(clicked(bool)), this, SLOT(readOutputs()));
    connect(ui->cmdConfigOut, SIGNAL(clicked(bool)), this, SLOT(writeConfiguration()));
    connect(ui->cmbConfig, SIGNAL(currentIndexChanged(int)), this, SLOT(configItemChanged()));
    connect(ui->cmdReset, SIGNAL(clicked(bool)), this, SLOT(runDioReset()));
    connect(tmrGoTimer, SIGNAL(timeout()), this, SLOT(runSelectedFunction()));
    createBitBoxes();
    mFirstCall = true;
}

DioForm::~DioForm()
{
    delete ui;
}

void DioForm::keyPressEvent(QKeyEvent *event)
{
    int keyCode = event->key();
    if (keyCode == Qt::Key_Escape) {
        toggleGoTimer(false);
    }
}

void DioForm::closeEvent(QCloseEvent *event)
{
    event->accept();
}

void DioForm::updateParameters()
{
    ChildWindow *parentWindow;
    parentWindow = qobject_cast<ChildWindow *>(this->parent());
    mAddress = parentWindow->devAddress();
    mDevName = parentWindow->devName();
    mHatID = parentWindow->devId();
    mCurGroup = parentWindow->curFunctionGroup();

    mUseTimer = parentWindow->tmrEnabled();
    //mGoTmrIsRunning = parentWindow->tmrRunning();
    mTmrInterval = parentWindow->tmrInterval();
    mOneSampPerForTotalSamps = parentWindow->tmrSampPerInterval();
    ui->lblInfo->setText(QString("Timer set to %1 with interval %2.")
                         .arg(mUseTimer)
                         .arg(mTmrInterval));

    initDeviceParams();
    if(!ui->stackedWidget->isVisible())
        parentWindow->adjustSize();
    //this->adjustSize();
    mGroupName = "[" + getFuncGroupName((UtFunctionGroup)mCurGroup) + "] ";
    this->setWindowTitle(mGroupName + mFuncName + ": " + mDevName);
}

void DioForm::groupChanged(int newGroup)
{
    mCurGroup = newGroup;
    mGroupName = "[" + getFuncGroupName((UtFunctionGroup)mCurGroup) + "] ";
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

    disableCheckboxInput(false);
    this->setUiForFunction();
}

void DioForm::setUiForFunction()
{
    bool scanVisible, portsVisible, stopVisible;
    bool scanParamsVisible, setNumberVisible, asyncVisible;
    bool configVisible, portCfgVisible, portSelVisible;
    bool readCommandVisible, stopEnable;
    int stackIndex;
    ChildWindow *parentWindow;

    parentWindow = qobject_cast<ChildWindow *>(this->parent());
    parentWindow->setTmrEnabled(false);

    portsVisible = false;
    portSelVisible = false;
    portCfgVisible = false;
    asyncVisible = false;
    scanVisible = false;
    scanParamsVisible = false;
    setNumberVisible = false;
    readCommandVisible = false;
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
    QString writeCommandText = "Write Configuration";
    QString readCommandText = "Read Output";
    //int defPort;
    disconnect(ui->cmdStop, SIGNAL(clicked(bool)), 0, 0);
    switch (mUtFunction) {
    case UL_D_CONFIG_PORT:
        asyncVisible = true;
        setNumberVisible = true;
        portsVisible = true;
        portCfgVisible = true;
        configVisible = true;
        stopVisible = true;
        if (mCurGroup == FUNC_GROUP_DOUT) {
            readCommandVisible = true;
            sampToolTip = "Value to write";
            goText = "Write";
        }
        mFuncName = "Port Config";
        stopText = "Read Cfg";
        stopEnable = true;
        //stop functions as read config in this mode
        connect(ui->cmdStop, SIGNAL(clicked(bool)), this, SLOT(readConfig()));
        break;
    case UL_D_CONFIG_BIT:
        asyncVisible = true;
        stopVisible = true;
        configVisible = true;
        setNumberVisible = true;
        stopEnable = true;
        stackIndex = 1;
        mFuncName = "Bit Config";
        stopText = "Read Cfg";
        sampToolTip = "Bit and Port to read/write (bit, port format)";
        setDefaultBits(0);
        connect(ui->cmdStop, SIGNAL(clicked(bool)), this, SLOT(readConfig()));
        break;
    case UL_D_IN:
        asyncVisible = true;
        setNumberVisible = true;
        mFuncName = "Read Port";
        startSample = "10";
        sampToolTip = "Samples per channel";
        connect(ui->cmdStop, SIGNAL(clicked(bool)), this, SLOT(onClickCmdStop()));
        break;
    case UL_D_OUT:
        portsVisible = true;
        asyncVisible = true;
        setNumberVisible = true;
        mFuncName = "Write Port";
        goText = "Write";
        break;
    case UL_D_BIT_IN:
        asyncVisible = true;
        disableCheckboxInput(true);
        setDefaultBits(0);
        stackIndex = 1;
        connect(ui->cmdStop, SIGNAL(clicked(bool)), this, SLOT(onClickCmdStop()));
        mFuncName = "Read Bit";
        break;
    case UL_D_BIT_OUT:
        asyncVisible = true;
        stackIndex = 1;
        setDefaultBits(0);
        mFuncName = "Write Bit";
        goText = "Read";
        break;
    case UL_D_INT_PORT:
        asyncVisible = true;
        setNumberVisible = true;
        mFuncName = "IntPortIn";
        startSample = "10";
        sampToolTip = "Samples per channel";
        connect(ui->cmdStop, SIGNAL(clicked(bool)), this, SLOT(onClickCmdStop()));
        break;
    case UL_D_INT_BIT:
        asyncVisible = true;
        disableCheckboxInput(true);
        setDefaultBits(0);
        stackIndex = 1;
        connect(ui->cmdStop, SIGNAL(clicked(bool)), this, SLOT(onClickCmdStop()));
        mFuncName = "IntBitIn";
        break;
    case UL_D_INT_WAIT:
        asyncVisible = true;
        setNumberVisible = true;
        mFuncName = "IntWait";
        startSample = "10";
        sampToolTip = "Timeout";
        break;
    /*case UL_D_INSCAN:
        mFuncName = "ulDInScan";
        asyncVisible = true;
        goText = "Go";
        sampToolTip = "Samples per channel";
        startSample = "1000";
        scanVisible = true;
        mPlot = true;
        setNumberVisible = true;
        stackIndex = 2;
        break;
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
    ui->fraPortSel->setVisible(portSelVisible);
    ui->fraPortCfg->setVisible(portCfgVisible);
    ui->cmdConfigIn->setVisible(readCommandVisible);
    ui->fraScan->setVisible(scanVisible);
    ui->fraAsync->setVisible(asyncVisible);
    ui->cmbConfig->setVisible(configVisible);
    ui->cmdConfigIn->setVisible(readCommandVisible);
    ui->spnLowChan->setVisible(scanParamsVisible);
    ui->spnHighChan->setVisible(scanParamsVisible);
    ui->leNumSamples->setVisible(setNumberVisible);
    ui->cmdStop->setVisible(stopVisible);
    ui->cmdStop->setEnabled(stopEnable);
    ui->stackedWidget->setCurrentIndex(stackIndex);
    ui->leNumSamples->setText(startSample);
    ui->leBlockSize->setText(blockText);
    ui->cmdConfigOut->setText(writeCommandText);
    ui->cmdConfigIn->setText(readCommandText);
    ui->cmdGo->setText(goText);
    ui->cmdStop->setText(stopText);
    ui->leRate->setText(rateVal);
    ui->leNumSamples->setToolTip(sampToolTip);
    if (mPlot)
        showPlotWindow(mPlot);
    ui->cmdGo->setFocus();
    if(!ui->stackedWidget->isVisible()) {
        //delay(300);
        parentWindow->adjustSize();
    }
    this->setWindowTitle(mGroupName + mFuncName + ": " + mDevName);
    parentWindow->setWindowState(Qt::WindowNoState);
}

void DioForm::showPlotWindow(bool showIt)
{
    QFrame::Shape frameShape;

    if (showIt)
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
    mNumBits = hatInterface->getNumDioChans(mHatID);
    if(mFirstCall) {
        mUtFunction = UL_D_CONFIG_PORT;
        setUiForFunction();
    }
    mFirstCall = false;
}

void DioForm::configItemChanged()
{
    mConfigItem = ui->cmbConfig->currentData().toUInt();
    readConfig();
}

void DioForm::writeConfiguration()
{
    uint8_t value;

    value = ui->leNumSamples->text().toUInt();
    mResponse = hatInterface->dioPortConfigWrite(mHatID, mAddress, mConfigItem, value);
    ui->lblInfo->setText(hatInterface->getStatus());
}

void DioForm::runDioReset()
{
    mResponse = hatInterface->dioReset(mHatID, mAddress);
    ui->lblInfo->setText(hatInterface->getStatus());
}

void DioForm::readOutputs()
{
    uint8_t value;

    value = 0;
    mResponse = hatInterface->dioOutPortRead(mHatID, mAddress, value);
    ui->leNumSamples->setText(QString("%1").arg(value));
    ui->lblInfo->setText(hatInterface->getStatus());
}

void DioForm::readConfig()
{
    uint8_t value;

    mResponse = hatInterface->dioPortConfigRead(mHatID, mAddress, mConfigItem, value);
    ui->leNumSamples->setText(QString("%1").arg(value));
    ui->lblInfo->setText(hatInterface->getStatus());

    if(mUtFunction == UL_D_CONFIG_BIT) {
        for (int i = 0; i < numBitCheckboxes; i++) {
            mResponse = hatInterface->dioBitConfigRead(mHatID, mAddress, i, mConfigItem, value);
            chkBit[i]->setChecked(value == 1);
        }
    }
}

void DioForm::onClickCmdGo()
{
    bool tmrEnable;

    tmrEnable = mUseTimer;
    switch (mUtFunction) {
    case UL_D_CONFIG_PORT:
        if(mCurGroup == FUNC_GROUP_DIN)
            readPort();
        else
            writePort();
        break;
    case UL_D_CONFIG_BIT:
        readPort();
        readBits();
        break;
    case UL_D_IN:
        runSelectedFunction();
        toggleGoTimer(tmrEnable);
        break;
    case UL_D_OUT:
        runSelectedFunction();
        toggleGoTimer(tmrEnable);
        break;
    case UL_D_BIT_IN:
        //disableCheckboxInput(true);
        //runDBitInFunc();
        runSelectedFunction();
        toggleGoTimer(tmrEnable);
        break;
    case UL_D_BIT_OUT:
        runDBitInFunc();
        break;
    case UL_D_INT_PORT:
        runSelectedFunction();
        toggleGoTimer(tmrEnable);
        break;
    case UL_D_INT_BIT:
        runSelectedFunction();
        toggleGoTimer(tmrEnable);
        break;
    case UL_D_INT_WAIT:
        waitForInterupt();
        break;
    default:
        runSelectedFunction();
        break;
    }
}

void DioForm::bitToggled(int bitNumber)
{
    bool setValue = chkBit[bitNumber]->isChecked();
    unsigned int bitVal = 0;
    if (setValue) {
        bitVal = 1;
    }

    switch (mUtFunction) {
    case UL_D_CONFIG_BIT:
        runDBitConfigFunc(bitNumber, bitVal );
        break;
    case UL_D_IN:
        break;
    case UL_D_OUT:
        //runDOutFunc();
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
    QFont goFont = ui->cmdGo->font();
    switch (mUtFunction) {
    case UL_D_CONFIG_PORT:
        //runDOutFunc();
        break;
    case UL_D_CONFIG_BIT:
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
        runDBitInFunc();
        break;
    case UL_D_BIT_OUT:
        //runDBitInFunc();
        break;
    case UL_D_INT_PORT:
        runIntPortFunc();
        break;
    case UL_D_INT_BIT:
        runIntBitFunc();
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
    if(mGoTmrIsRunning) {
        goFont.setBold(!ui->cmdGo->font().bold());
        ui->cmdGo->setFont(goFont);
    }
}

void DioForm::readPort()
{
    uint8_t value;

    if(mCurGroup == FUNC_GROUP_DIN)
        mResponse = hatInterface->dioInPortRead(mHatID, mAddress, value);
    else
        mResponse = hatInterface->dioOutPortRead(mHatID, mAddress, value);

    ui->leNumSamples->setText(QString("%1").arg(value));
    ui->lblInfo->setText(hatInterface->getStatus());
}

void DioForm::readBits()
{
    uint8_t bit, value;

    for(bit = 0; bit < numBitCheckboxes; bit++) {
        if(mCurGroup == FUNC_GROUP_DIN)
            mResponse = hatInterface->dioInBitRead(mHatID, mAddress, bit, value);
        else
            mResponse = hatInterface->dioOutBitRead(mHatID, mAddress, bit, value);
        ui->lblInfo->setText(hatInterface->getStatus());
        chkBit[bit]->setChecked(value != 0);
    }
}

void DioForm::runDInFunc()
{
    uint8_t value;
    int numSamples;

    ui->teShowValues->clear();
    numSamples = ui->leNumSamples->text().toInt();
    if(mCurGroup == FUNC_GROUP_DIN) {
        for(int i = 0; i < numSamples; i++) {
            mResponse = hatInterface->dioInPortRead(mHatID, mAddress, value);
            ui->teShowValues->append(QString("%1").arg(value));
        }
    } else {
        mResponse = hatInterface->dioOutPortRead(mHatID, mAddress, value);
        ui->teShowValues->setText(QString("%1").arg(value));
    }
    ui->lblInfo->setText(hatInterface->getStatus());
}

void DioForm::runIntPortFunc()
{
    uint8_t value;

    ui->teShowValues->clear();
    mResponse = hatInterface->dioIntStatusPort(mHatID, mAddress, value);
    ui->teShowValues->setText(QString("%1").arg(value));
    ui->lblInfo->setText(hatInterface->getStatus());
}

void DioForm::runDBitInFunc()
{
    uint8_t bit, value;

    for(bit = 0; bit < numBitCheckboxes; bit++) {
        mResponse = hatInterface->dioInBitRead(mHatID, mAddress, bit, value);
        ui->lblInfo->setText(hatInterface->getStatus());
        chkBit[bit]->setChecked(value != 0);
    }
}

void DioForm::runIntBitFunc()
{
    uint8_t bit, value;

    for(bit = 0; bit < numBitCheckboxes; bit++) {
        mResponse = hatInterface->dioIntStatusBit(mHatID, mAddress, bit, value);
        ui->lblInfo->setText(hatInterface->getStatus());
        chkBit[bit]->setChecked(value != 0);
    }
}

void DioForm::waitForInterupt()
{
    int timeout, intState;
    QString waitStatus, timoText;

    timeout = ui->leNumSamples->text().toInt();
    if(timeout == (-1))
        timoText = "indefinitely.\n";
    else
        timoText = QString("for %1ms.\n").arg(timeout);
    ui->teShowValues->setText("Waiting for interrupt " + timoText);
    delay(200);
    mResponse = hatInterface->waitForInterrupt(timeout);
    waitStatus = hatInterface->getStatus();
    ui->lblInfo->setText(waitStatus);
    ui->teShowValues->append(waitStatus);
    intState = hatInterface->getInterruptState();
    ui->teShowValues->append(QString("Interrupt state: %1").arg(intState));
    ui->lblStatus->setText(hatInterface->getStatus());
}

void DioForm::writePort()
{
    uint8_t value;

    value = ui->leNumSamples->text().toUInt();
    mResponse = hatInterface->dioOutPortWrite(mHatID, mAddress, value);
    ui->lblInfo->setText(hatInterface->getStatus());
}

void DioForm::runDOutFunc()
{
    uint8_t value;

    ui->teShowValues->clear();
    value = ui->leNumSamples->text().toUInt();
    mResponse = hatInterface->dioOutPortWrite(mHatID, mAddress, value);
    ui->lblInfo->setText(hatInterface->getStatus());
}

void DioForm::runDBitOutFunc(uint8_t bit, uint8_t value)
{
    mResponse = hatInterface->dioOutBitWrite(mHatID, mAddress, bit, value);
    ui->lblInfo->setText(hatInterface->getStatus());
}

void DioForm::runDBitConfigFunc(uint8_t bit, uint8_t value)
{
    mResponse = hatInterface->dioBitConfigWrite(mHatID, mAddress, bit, mConfigItem, value);
    ui->lblInfo->setText(hatInterface->getStatus());
}

void DioForm::onClickCmdStop()
{
    toggleGoTimer(false);
}

void DioForm::toggleGoTimer(bool enableTimer)
{
    ChildWindow *parentWindow;
    QFont goFont = ui->cmdGo->font();

    parentWindow = qobject_cast<ChildWindow *>(this->parent());
    if(enableTimer) {
        parentWindow->setTmrRunning(true);
        goFont.setBold(true);
    } else {
        parentWindow->setTmrRunning(false);
        goFont.setBold(false);
    }
    mGoTmrIsRunning = enableTimer;
    ui->cmdStop->setVisible(mUseTimer);
    ui->cmdStop->setEnabled(enableTimer);
    ui->cmdGo->setFont(goFont);
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
    bitLayout->setSizeConstraint(QLayout::SetFixedSize); //
    //bitLayout->setColumnMinimumWidth(3, 40);

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
        chkBit[i]->setStyleSheet("QCheckBox::indicator { color: red; }" );

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
        bitLayout->addWidget(chkBit[i], pos % 4, pos / 4);
        pos++;
    }
    ui->stackedWidget->widget(1)->setLayout(bitLayout);
}

void DioForm::disableCheckboxInput(bool disableMouse)
{
    if (disableMouse)
        for (int i = 0; i < numBitCheckboxes; i++) {
            chkBit[i]->setAttribute(Qt::WA_TransparentForMouseEvents);
        }
    else
        for (int i = 0; i < numBitCheckboxes; i++) {
            chkBit[i]->setAttribute(Qt::WA_TransparentForMouseEvents, false);
        }
}

void DioForm::setDefaultBits(uint8_t portType)
{
    (void)portType;
    uint8_t dirValue, bitVal;
    bool checkTrue;

    mResponse = hatInterface->dioPortConfigRead(mHatID, mAddress, DIO_DIRECTION, dirValue);
    /*portIndex = validPorts.indexOf(portType);
    configMask = getIOConfigMask(portIndex);
    int numBits = portBits.value(portType);
    int portOffset = getGridOffsetFromPort(portType);
    */
    checkTrue = false;
    for (int i = 0; i < numBitCheckboxes; i++) {
        bool triState = true;
        if ((mUtFunction == UL_D_CONFIG_BIT) |
            (mUtFunction == UL_D_BIT_OUT)) {
            triState = false;
            if (mUtFunction == UL_D_CONFIG_BIT)
                checkTrue = ((int)pow(2, i) & (int)dirValue);
        }
        if(mUtFunction == UL_D_BIT_IN) {
            chkBit[i]->setTristate(false);
            mResponse = hatInterface->dioInBitRead(mHatID, mAddress, i, bitVal);
            chkBit[i]->setChecked(bitVal == 1);
        } else {
            chkBit[i]->setTristate(triState);
            chkBit[i]->setChecked(checkTrue);
        }
    }
}
