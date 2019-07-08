#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "childwindow.h"
#include "hatinterface.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->cmdDiscover, SIGNAL(clicked(bool)), this, SLOT(createDiscChild()));
    connect(ui->cmdConfig, SIGNAL(clicked(bool)), this, SLOT(createInfoChild()));
    connect(ui->cmdAIn, SIGNAL(clicked(bool)), this, SLOT(createAiChild()));
    connect(ui->cmdAOut, SIGNAL(clicked(bool)), this, SLOT(createAoChild()));
    connect(ui->cmdDIn, SIGNAL(clicked(bool)), this, SLOT(createDinChild()));
    connect(ui->cmdDOut, SIGNAL(clicked(bool)), this, SLOT(createDOutChild()));
    connect(ui->cmdMisc, SIGNAL(clicked(bool)), this, SLOT(createMiscChild()));
    connect(ui->cmdHistory, SIGNAL(clicked(bool)), this, SLOT(showHistory()));
    connect(ui->actionVolts_vs_Time, SIGNAL(triggered(bool)), this, SLOT(showPlot(bool)));
    connect(ui->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(setBoardMenuSelect(QMdiSubWindow*)));
    connect(ui->actionLoad_Queue, SIGNAL(triggered(bool)), this, SLOT(configureQueue()));
    connect(ui->actionTmrloop, SIGNAL(triggered(bool)), this, SLOT(setTimer()));

    mHistListSize = 50;
    mScanOptions = OPTS_IFC_BACKGROUND;
    readWindowPosition();

    functionGroup = new QActionGroup(this);
    createFuncMenus();

    //QAction *funcAction;
    /*
    funcAction = ui->menuFunction->addAction("AIn");
    funcAction->setCheckable(true);
    funcAction->setChecked(true);
    funcAction->setData(UL_AIN);
    functionGroup->addAction(funcAction);
    funcAction = ui->menuFunction->addAction("AInScan");
    funcAction->setCheckable(true);
    funcAction->setData(UL_AINSCAN);
    functionGroup->addAction(funcAction);
    */
    //functionGroup->actions().clear();

    trigTypeGroup = new QActionGroup(this);
    trigTypeGroup->addAction(ui->actionRISING_EDGE);
    trigTypeGroup->addAction(ui->actionFALLING_EDGE);
    trigTypeGroup->addAction(ui->actionACTIVE_HIGH);
    trigTypeGroup->addAction(ui->actionACTIVE_LOW);

    trigSourceGroup = new QActionGroup(this);
    trigSourceGroup->addAction(ui->action_SOURCE_LOCAL);
    trigSourceGroup->addAction(ui->actionSOURCE_MASTER);
    trigSourceGroup->addAction(ui->actionSOURCE_SLAVE);

    ui->actionRISING_EDGE->setData(TRIG_RISING_EDGE);
    ui->actionFALLING_EDGE->setData(TRIG_FALLING_EDGE);
    ui->actionACTIVE_HIGH->setData(TRIG_ACTIVE_HIGH);
    ui->actionACTIVE_LOW->setData(TRIG_ACTIVE_LOW);
    connect(trigTypeGroup, SIGNAL(triggered(QAction*)), this, SLOT(changeTrigType()));

    ui->action_SOURCE_LOCAL->setData(SOURCE_LOCAL);
    ui->actionSOURCE_MASTER->setData(SOURCE_MASTER);
    ui->actionSOURCE_SLAVE->setData(SOURCE_SLAVE);
    connect(trigSourceGroup, SIGNAL(triggered(QAction*)), this, SLOT(changeSourceType()));

    ui->actionNOCALIBRATEDATA->setData(OPTS_NOCALIBRATEDATA);
    ui->actionNOSCALEDATA->setData(OPTS_NOSCALEDATA);
    ui->actionCONTINUOUS->setData(OPTS_CONTINUOUS);
    ui->actionEXTCLOCK->setData(OPTS_EXTCLOCK);
    ui->actionEXTTRIGGER->setData(OPTS_EXTTRIGGER);
    ui->actionBACKGROUND->setData(OPTS_IFC_BACKGROUND);

    connect(functionGroup, SIGNAL(triggered(QAction*)), this, SLOT(curFunctionChanged()));

    connect(ui->actionCONTINUOUS, SIGNAL(triggered(bool)), this, SLOT(curOptionChanged()));
    connect(ui->actionEXTCLOCK, SIGNAL(triggered(bool)), this, SLOT(curOptionChanged()));
    connect(ui->actionEXTTRIGGER, SIGNAL(triggered(bool)), this, SLOT(curOptionChanged()));
    connect(ui->actionNOCALIBRATEDATA, SIGNAL(triggered(bool)), this, SLOT(curOptionChanged()));
    connect(ui->actionNOSCALEDATA, SIGNAL(triggered(bool)), this, SLOT(curOptionChanged()));
    connect(ui->actionBACKGROUND, SIGNAL(triggered(bool)), this, SLOT(curOptionChanged()));

    mHatList.clear();

#ifdef HAT_04
    this->setWindowTitle("MCC HAT Test [0.4]");
    ui->cmdAOut->setEnabled(true);
    ui->cmdDIn->setEnabled(true);
    ui->cmdDOut->setEnabled(true);
    ui->cmdMisc->setEnabled(true);
#elif HAT_03
    this->setWindowTitle("MCC HAT Test [0.3]");
#else
    this->setWindowTitle("MCC HAT Test [0.2]");
#endif

    if(ui->chkAutoDetect->isChecked())
        readHatList();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    HatInterface *hatInterface;
    hatInterface = new HatInterface;
    uint8_t address;
    uint16_t devID;

    foreach (address, mHatList.keys()) {
        devID = mHatIDList.value(address);
        hatInterface->closeDevice(devID, address);
    }
    writeWindowPosition();
    hatInterface = NULL;
    event->accept();
}

ChildWindow *MainWindow::activeMdiChild() const
{
    if (QMdiSubWindow *activeSubWindow = ui->mdiArea->activeSubWindow())
        return qobject_cast<ChildWindow *>(activeSubWindow);
    return 0;
}

void MainWindow::setTimer()
{
    ChildWindow *curChild;

    curChild = activeMdiChild();
    if (curChild) {
        curChild->setUpTimer();
    }
}

void MainWindow::readHatList()
{
    HatInterface *hatInterface;
    hatInterface = new HatInterface;
    uint16_t hatFilter;
    uint16_t devId;
    uint8_t address;
    int numHats, hatIndex, response;
    QString devName;

    hatFilter = HAT_ID_ANY;
    numHats = hat_list(hatFilter, hatInfoList);

    for(hatIndex = 0; hatIndex < numHats; hatIndex++) {
        address = hatInfoList[hatIndex].address;
        devId = hatInfoList[hatIndex].id;
        response = hatInterface->openDevice(devId, address);
        if (response == RESULT_SUCCESS) {
            devName = QString(hatInfoList[hatIndex].product_name).left(7)
                    + QString(" [%1]").arg(address);
            addDeviceToMenu(devName, address, devId);
        }
    }

}

void MainWindow::createDiscChild()
{
    curFunctionGroupName = "Discovery Functions";
    createChild(FUNC_GROUP_DISC, UL_DISC);
}

void MainWindow::createInfoChild()
{
    curFunctionGroupName = "Hat Info";
    createChild(FUNC_GROUP_MISC, UL_AI_INFO);
}

void MainWindow::createAiChild()
{
    curFunctionGroupName = "Analog Input";
    createChild(FUNC_GROUP_AIN, UL_AIN);
}

void MainWindow::createAoChild()
{
    curFunctionGroupName = "Analog Output";
    createChild(FUNC_GROUP_AOUT, UL_AOUT);
}

void MainWindow::createDinChild()
{
    curFunctionGroupName = "Digital Input";
    createChild(FUNC_GROUP_DIN, UL_D_CONFIG_PORT);
}

void MainWindow::createDOutChild()
{
    curFunctionGroupName = "Digital Output";
    createChild(FUNC_GROUP_DOUT, UL_D_CONFIG_PORT);
}

void MainWindow::createMiscChild()
{
    curFunctionGroupName = "Status";
    createChild(FUNC_GROUP_STATUS, UL_GET_STATUS);
}

void MainWindow::createChild(UtFunctionGroup utFuncGroup, int defaultFunction)
{
    foreach (QAction *devAct, ui->menuBoards->actions()) {
        if (devAct->isChecked()) {
            mCurAddress = devAct->data().toUInt();
            break;
        }
    }

    if(mHatList.contains(mCurAddress)) {
        mCurBoardName = mHatList.value(mCurAddress);
    }
    ui->lblAppStatus->setText("Device: " + mCurBoardName);
    if(mHatIDList.contains(mCurAddress))
        mCurID = mHatIDList.value(mCurAddress);

    ChildWindow *childWindow = new ChildWindow(ui->mdiArea, utFuncGroup);
    childWindow->setWindowState(Qt::WindowMaximized);
    childWindow->setWindowState(Qt::WindowActive);
    childWindow->show();
    childWindow->setDevAddress(mCurAddress);
    childWindow->setDevName(mCurBoardName);
    childWindow->setDevId(mCurID);
    childWindow->setCurFunction(defaultFunction);
    if(utFuncGroup == FUNC_GROUP_AIN)
        childWindow->setScanOptions(OPTS_IFC_BACKGROUND);
    createFuncMenus();
}

void MainWindow::createFuncMenus()
{
    QAction *funcAction;
    ChildWindow *curChild;
    bool optionVisible, trigVisible;
    bool plotVisible, rangeVisible;

    functionGroup->actions().clear();
    optionVisible = false;
    rangeVisible = true;
    trigVisible = true;
    plotVisible = true;
    curChild = activeMdiChild();

    if (curChild) {
        int funcGroup = curChild->curFunctionGroup();
        ui->menuFunction->clear();

        switch (funcGroup) {
        case FUNC_GROUP_AIN:
            optionVisible = true;
            funcAction = ui->menuFunction->addAction("AIn");
            funcAction->setCheckable(true);
            funcAction->setChecked(true);
            funcAction->setData(UL_AIN);
            functionGroup->addAction(funcAction);
            funcAction = ui->menuFunction->addAction("AInScan");
            funcAction->setCheckable(true);
            funcAction->setData(UL_AINSCAN);
            functionGroup->addAction(funcAction);
            funcAction = ui->menuFunction->addAction("TIn");
            funcAction->setCheckable(true);
            funcAction->setData(UL_TIN);
            functionGroup->addAction(funcAction);
            break;
        case FUNC_GROUP_AOUT:
            optionVisible = true;
            trigVisible = false;
            plotVisible = false;
            //dataVisible = true;
            funcAction = ui->menuFunction->addAction("ulAOut");
            funcAction->setCheckable(true);
            funcAction->setChecked(true);
            funcAction->setData(UL_AOUT);
            functionGroup->addAction(funcAction);
            funcAction = ui->menuFunction->addAction("ulAOutScan");
            funcAction->setCheckable(true);
            funcAction->setData(UL_AOUT_SCAN);
            functionGroup->addAction(funcAction);
            break;
        case FUNC_GROUP_DIN:
            optionVisible = true;
            rangeVisible = false;
            trigVisible = false;
            plotVisible = false;
            //flagsVisible = false;
            funcAction = ui->menuFunction->addAction("Configure Port");
            funcAction->setCheckable(true);
            funcAction->setChecked(true);
            functionGroup->addAction(funcAction);
            funcAction->setData(UL_D_CONFIG_PORT);
            funcAction = ui->menuFunction->addAction("Configure Bits");
            funcAction->setCheckable(true);
            functionGroup->addAction(funcAction);
            funcAction->setData(UL_D_CONFIG_BIT);
            funcAction = ui->menuFunction->addAction("Read Port");
            funcAction->setCheckable(true);
            functionGroup->addAction(funcAction);
            funcAction->setData(UL_D_IN);
            funcAction = ui->menuFunction->addAction("Read Bits");
            funcAction->setCheckable(true);
            functionGroup->addAction(funcAction);
            funcAction->setData(UL_D_BIT_IN);
            funcAction = ui->menuFunction->addAction("Interrupt Port Read");
            funcAction->setCheckable(true);
            functionGroup->addAction(funcAction);
            funcAction->setData(UL_D_INT_PORT);
            funcAction = ui->menuFunction->addAction("Interrupt Bit Read");
            funcAction->setCheckable(true);
            functionGroup->addAction(funcAction);
            funcAction->setData(UL_D_INT_BIT);
            funcAction = ui->menuFunction->addAction("Interrupt Wait");
            funcAction->setCheckable(true);
            functionGroup->addAction(funcAction);
            funcAction->setData(UL_D_INT_WAIT);
            break;
        case FUNC_GROUP_DOUT:
            optionVisible = true;
            rangeVisible = false;
            trigVisible = false;
            plotVisible = false;
            //flagsVisible = false;
            //dataVisible = true;
            funcAction = ui->menuFunction->addAction("Configure Port");
            funcAction->setCheckable(true);
            funcAction->setChecked(true);
            functionGroup->addAction(funcAction);
            funcAction->setData(UL_D_CONFIG_PORT);
            funcAction = ui->menuFunction->addAction("Configure Bits");
            funcAction->setCheckable(true);
            functionGroup->addAction(funcAction);
            funcAction->setData(UL_D_CONFIG_BIT);
            funcAction = ui->menuFunction->addAction("Write Port");
            funcAction->setCheckable(true);
            functionGroup->addAction(funcAction);
            funcAction->setData(UL_D_OUT);
            funcAction = ui->menuFunction->addAction("Write Bits");
            funcAction->setCheckable(true);
            functionGroup->addAction(funcAction);
            funcAction->setData(UL_D_BIT_OUT);
            //funcAction = ui->menuFunction->addAction("ulDOutScan");
            //funcAction->setCheckable(true);
            //functionGroup->addAction(funcAction);
            //funcAction->setData(UL_D_OUTSCAN);
            break;
        /*
        case FUNC_GROUP_CTR:
            optionVisible = true;
            funcAction = ui->menuFunction->addAction("ulCLoad");
            funcAction->setCheckable(true);
            funcAction->setChecked(true);
            functionGroup->addAction(funcAction);
            funcAction->setData(UL_C_LOAD);
            funcAction = ui->menuFunction->addAction("ulCIn");
            funcAction->setCheckable(true);
            functionGroup->addAction(funcAction);
            funcAction->setData(UL_C_IN);
            funcAction = ui->menuFunction->addAction("ulCClear");
            funcAction->setCheckable(true);
            functionGroup->addAction(funcAction);
            funcAction->setData(UL_C_CLEAR);
            funcAction = ui->menuFunction->addAction("ulCInScan");
            funcAction->setCheckable(true);
            functionGroup->addAction(funcAction);
            funcAction->setData(UL_C_INSCAN);
            funcAction = ui->menuFunction->addAction("ulTmrPulseOut");
            funcAction->setCheckable(true);
            functionGroup->addAction(funcAction);
            funcAction->setData(UL_TMR_OUT);
            funcAction = ui->menuFunction->addAction("ulCConfigScan");
            funcAction->setCheckable(true);
            functionGroup->addAction(funcAction);
            funcAction->setData(UL_C_CONFIG_SCAN);
            ui->actionFF_DEFAULT->setText("FF_DEFAULT");
            ui->actionFF_NOCALIBRATEDATA->setText("FF_CTR16_BIT");
            ui->actionFF_NOSCALEDATA->setText("FF_CTR32_BIT");
            ui->actionFF_DEFAULT->setData(CINSCAN_FF_DEFAULT);
            ui->actionFF_NOCALIBRATEDATA->setData(CINSCAN_FF_CTR16_BIT);
            ui->actionFF_NOSCALEDATA->setData(CINSCAN_FF_CTR32_BIT);
            ui->actionFF_CTR64_BIT->setData(CINSCAN_FF_CTR64_BIT);
            ui->actionFF_CTR64_BIT->setVisible(true);
            ui->actionFF_NOCLEAR->setData(CINSCAN_FF_NOCLEAR);
            ui->actionFF_NOCLEAR->setVisible(true);
            break;
            */
        case FUNC_GROUP_MISC:
            trigVisible = false;
            plotVisible = false;
            funcAction = ui->menuFunction->addAction("Utilities");
            funcAction->setCheckable(true);
            funcAction->setData(UL_GET_ERR_MSG);
            functionGroup->addAction(funcAction);
            funcAction = ui->menuFunction->addAction("Analog Input");
            funcAction->setCheckable(true);
            funcAction->setChecked(true);
            funcAction->setData(UL_AI_INFO);
            functionGroup->addAction(funcAction);
            funcAction = ui->menuFunction->addAction("Temperature Input");
            funcAction->setCheckable(true);
            funcAction->setData(UL_TEMP_INFO);
            functionGroup->addAction(funcAction);
            funcAction = ui->menuFunction->addAction("Test Device");
            funcAction->setCheckable(true);
            funcAction->setData(UL_TEST);
            functionGroup->addAction(funcAction);
            trigVisible = false;
            plotVisible = false;
            break;
        case FUNC_GROUP_STATUS:
            optionVisible = true;
            rangeVisible = false;
            trigVisible = false;
            plotVisible = false;
            break;
        /*
        case FUNC_GROUP_CONFIG:
            flagsVisible = false;
            trigVisible = false;
            plotVisible = false;
            funcAction = ui->menuFunction->addAction("ulGetInfo");
            funcAction->setCheckable(true);
            funcAction->setChecked(true);
            funcAction->setData(UL_GET_INFO);
            functionGroup->addAction(funcAction);
            funcAction = ui->menuFunction->addAction("ulGetConfig");
            funcAction->setCheckable(true);
            funcAction->setData(UL_GET_CONFIG);
            functionGroup->addAction(funcAction);
            funcAction = ui->menuFunction->addAction("ulSetConfig");
            funcAction->setCheckable(true);
            funcAction->setData(UL_SET_CONFIG);
            functionGroup->addAction(funcAction);
            break;
        default:
            break;
            */
        }
        disconnect(functionGroup);
        connect(functionGroup, SIGNAL(triggered(QAction*)), this, SLOT(curFunctionChanged()));
    }
    ui->menuOptions->menuAction()->setVisible(optionVisible);
    ui->menuTriggering->menuAction()->setVisible(trigVisible);
    ui->menuPlot->menuAction()->setVisible(plotVisible);
    ui->menuRange->menuAction()->setVisible(rangeVisible);
}

void MainWindow::setBoardMenuSelect(QMdiSubWindow * childWind)
{
    bool optionVisible, rangeVisible, trigVisible;
    bool plotShowing, plotVisible;
    int curFunc;
    QString curBoard;
    uint8_t curAddress;
    QString actionName;
    ChildWindow *curChild;

    trigVisible = true;
    plotVisible = true;
    optionVisible = false;
    plotShowing = false;
    rangeVisible = false;
    //curChild = activeMdiChild();
    curChild = qobject_cast<ChildWindow *>(childWind);
    ui->lblAppStatus->clear();
    if (curChild) {
        curBoard = curChild->devName();
        UtFunctionGroup functionGroup = curChild->curFunctionGroup();
        curFunctionGroupName = getFuncGroupName(functionGroup);
        if (curBoard == "") {
            ui->lblAppStatus->setText(curFunctionGroupName + ": No device name obtained");
            return;
        } else {
            ui->lblAppStatus->setText(curFunctionGroupName + ": " + curBoard);
        }
        curAddress = curChild->devAddress();
        //QString tempString;
        createFuncMenus();
        foreach (QAction *boardAction, ui->menuBoards->actions()) {
            QString bdName = boardAction->text();
            if (bdName == curBoard) {
                uint8_t curMenuAddress = boardAction->data().toUInt();
                if (curAddress == curMenuAddress) {
                    boardAction->setChecked(true);
                    mCurBoardName = curBoard;
                    mCurAddress = curAddress;
                } else
                    boardAction->setChecked(false);
            } else
                boardAction->setChecked(false);
        }
        UtFunctionGroup curFuncGroup = curChild->curFunctionGroup();
        curFunc = curChild->curFunction();
        curFunctionGroupName = getFuncGroupName(curFuncGroup);
        //mScanOptions = curChild->scanOptions();
        switch (curFuncGroup) {
        case FUNC_GROUP_AIN:
            optionVisible = true;
            rangeVisible = true;
            switch (curFunc) {
            case UL_AIN:
                actionName = "AIn";
                break;
            case UL_AINSCAN:
                actionName = "AInScan";
                break;
            case UL_TIN:
                actionName = "TIn";
            default:
                actionName = "AIn";
                break;
            }
            break;
        case FUNC_GROUP_AOUT:
            optionVisible = true;
            rangeVisible = true;
            trigVisible = false;
            plotVisible = false;
            switch (curFunc) {
            case UL_AOUT:
                actionName = "AOut";
                break;
            case UL_AOUT_SCAN:
                actionName = "AOutScan";
                break;
            default:
                actionName = "AOut";
                break;
            }
            break;
        case FUNC_GROUP_DIN:
            optionVisible = true;
            trigVisible = false;
            plotVisible = false;
            switch (curFunc) {
            case UL_D_CONFIG_PORT:
                actionName = "Configure Port";
                break;
            case UL_D_CONFIG_BIT:
                actionName = "Configure Bits";
                break;
            case UL_D_IN:
                actionName = "Read Port";
                break;
            case UL_D_BIT_IN:
                actionName = "Read Bits";
                break;
            case UL_D_INT_PORT:
                actionName = "Interrupt Port Read";
                break;
            case UL_D_INT_BIT:
                actionName = "Interrupt Bit Read";
                break;
            case UL_D_INT_WAIT:
                actionName = "Interrupt Wait";
                break;
            default:
                actionName = "PortConfig";
                break;
            }
            break;
        case FUNC_GROUP_DOUT:
            optionVisible = true;
            trigVisible = false;
            plotVisible = false;
            switch (curFunc) {
            case UL_D_CONFIG_PORT:
                actionName = "Configure Port";
                break;
            case UL_D_CONFIG_BIT:
                actionName = "Configure Bits";
                break;
            case UL_D_OUT:
                actionName = "Write Port";
                break;
            case UL_D_BIT_OUT:
                actionName = "Write Bits";
                break;
            default:
                actionName = "Configure Port";
                break;
            }
            break;
        case FUNC_GROUP_MISC:
            optionVisible = false;
            trigVisible = false;
            plotVisible = false;
            switch (curFunc) {
            default:
                break;
            }
            break;
        case FUNC_GROUP_CONFIG:
            optionVisible = true;
            trigVisible = false;
            plotVisible = false;
            switch (curFunc) {
            case UL_D_CONFIG_PORT:
                actionName = "PortConfig";
                break;
            case UL_D_CONFIG_BIT:
                actionName = "BitConfig";
                break;
            case UL_D_IN:
                actionName = "DIn";
                break;
            case UL_D_BIT_IN:
                actionName = "DBitIn";
                break;
            default:
                actionName = "PortConfig";
                break;
            }
            break;
        case FUNC_GROUP_DISC:
            optionVisible = false;
            break;
        case FUNC_GROUP_STATUS:
            optionVisible = true;
        default:
            break;
        }
        foreach (QAction *funcAction, ui->menuFunction->actions()) {
            if (funcAction->text() == actionName) {
                funcAction->setChecked(true);
                break;
            }
        }
        if (optionVisible) {
            uint32_t childOption = curChild->scanOptions();
            foreach (QAction *ioMode, ui->menuOptions->actions()) {
                uint32_t curMenuVal = (uint32_t)(ioMode->data().toInt());
                //tempString += QString("%1, %2;").arg(curMenuVal).arg(childOption);
                //ui->lblAppStatus->setText(tempString);
                //if(ioMode->text() != "BACKGROUND")
                ioMode->setChecked(false);
                if (curMenuVal & childOption) {
                    ioMode->setChecked(true);
                }
            }
        }
        TriggerMode childTrig = curChild->triggerType();
        foreach (QAction *trigMode, ui->menuTriggering->actions()) {
            TriggerMode curMenuTrig = (TriggerMode)(trigMode->data().toInt());
            if(curMenuTrig == childTrig)
                trigMode->setChecked(true);
        }
        plotShowing = curChild->showPlot();
    }
    ui->actionVolts_vs_Time->setChecked(plotShowing);
    ui->menuOptions->menuAction()->setVisible(optionVisible);
    ui->menuRange->menuAction()->setVisible(rangeVisible);
    ui->menuTriggering->menuAction()->setVisible(trigVisible);
    ui->menuPlot->menuAction()->setVisible(plotVisible);
}

void MainWindow::setSelectedDevice()
{
    ChildWindow *mdiChild = activeMdiChild();

    foreach (QAction *devAct, ui->menuBoards->actions()) {
        devAct->setChecked(false);
    }
    QObject *devSelected = this->sender();
    QAction *actDevSelected = qobject_cast<QAction *>(devSelected);
    devSelected->setProperty("checked", true);
    QVariant addressVar = actDevSelected->data();
    mCurAddress = addressVar.toUInt();
    mCurBoardName = mHatList.value(mCurAddress);
    mCurID = mHatIDList.value(mCurAddress);

    if(mdiChild) {
        mdiChild->setDevAddress(mCurAddress);
        mdiChild->setDevName(mCurBoardName);
        mdiChild->setDevId(mCurID);
        ui->lblAppStatus->setText(curFunctionGroupName + ": " + mCurBoardName + QString(" {%1}").arg(mCurID));
    }
}

void MainWindow::curFunctionChanged()
{
    int curFunction;
    ChildWindow *curChild;

    curChild = activeMdiChild();
    if (curChild) {
        curFunction = functionGroup->checkedAction()->data().toInt();
        curChild->setCurFunction(curFunction);
    }
}

void MainWindow::curOptionChanged()
{
    ChildWindow *curChild = activeMdiChild();

    QAction *menuOption = (QAction *)this->sender();
    if(menuOption->objectName() != "actionTmrloop") {
        QVariant optionValue = menuOption->data();
        bool enableOption = menuOption->isChecked();

        uint32_t optVal = (uint32_t)optionValue.toUInt();

        if (enableOption)
            mScanOptions = (uint32_t)(mScanOptions | optVal);
        else {
            uint32_t mask = getSoMask(optVal);
            mScanOptions = (uint32_t)(mScanOptions & mask);
        }
        if (curChild) {
            curChild->setScanOptions(mScanOptions);
        }
    }
}

uint32_t MainWindow::getSoMask(uint32_t optSelected)
{
    uint32_t allOpts = (uint32_t)(
                OPTS_CONTINUOUS | OPTS_EXTCLOCK |
                OPTS_EXTTRIGGER | OPTS_NOCALIBRATEDATA |
                OPTS_NOSCALEDATA | OPTS_IFC_BACKGROUND);
    uint32_t maskVal = (uint32_t)(allOpts ^ optSelected);
    return maskVal;
}

void MainWindow::configureQueue()
{
    ChildWindow *curChild = activeMdiChild();

    if (curChild) {
        curChild->showQueueConfig();
    }
}

void MainWindow::changeTrigType()
{
    ChildWindow *curChild = activeMdiChild();

    QVariant trigSetVal = trigTypeGroup->checkedAction()->data();
    mTriggerType = (TriggerMode)trigSetVal.toLongLong();

    if (curChild)
        curChild->setTriggerType(mTriggerType);
}

void MainWindow::changeSourceType()
{
    ChildWindow *curChild = activeMdiChild();

    QVariant trigSetSource = trigSourceGroup->checkedAction()->data();
    mTriggerSource = (uint8_t)trigSetSource.toLongLong();

    if (curChild)
        curChild->setTriggerSource(mTriggerSource);
}

void MainWindow::addDeviceToMenu(QString devName, uint8_t devAddress, uint16_t hatType)
{
    QAction *newAction, *checkedAction;
    bool itemExists;

    itemExists = false;
    checkedAction = NULL;
    //devList.insert(devUiD, devHandle);
    foreach (QAction *bAct, ui->menuBoards->actions()) {
        uint8_t menuDevAddress = bAct->data().toUInt();
        if (menuDevAddress == devAddress) {
            itemExists = true;
            break;
        }
        if (bAct->isChecked()) {
            bAct->setChecked(false);
            checkedAction = bAct;
        }
    }
    if (itemExists) {
        if (checkedAction)
            checkedAction->setChecked(true);
        return;
    }

    newAction = ui->menuBoards->addAction(devName);
    newAction->setCheckable(true);
    newAction->setData(devAddress);
    newAction->setChecked(true);
    mHatList.insert(devAddress, devName);
    mHatIDList.insert(devAddress, hatType);
    mCurID = hatType;
    connect(newAction, SIGNAL(triggered(bool)), this, SLOT(setSelectedDevice()));
    ui->lblAppStatus->setText(QString("Hatlist size: %1").arg(mHatList.count()));
}

void MainWindow::removeDeviceFromMenu(uint8_t devAddress)
{
    bool wasChecked;

    wasChecked = false;
    foreach (QAction *boardMenu, ui->menuBoards->actions()) {
        QVariant dataVal = boardMenu->data();
        if (dataVal.toInt() == devAddress) {
            if (mCurAddress == devAddress) {
                mCurBoardName = "";
                mCurAddress = -1;
            }
            //devList.remove(devUiD);
            wasChecked = boardMenu->isChecked();
            delete boardMenu;
        }
        if (wasChecked)
            if (ui->menuBoards->actions().count())
                ui->menuBoards->actions().at(0)->setChecked(true);
    }
    mHatList.remove(devAddress);
    mHatIDList.remove(devAddress);
}

void MainWindow::addFunction(QString funcString)
{
    mFuncHistoryList.append(funcString);
    if (mFuncHistoryList.size() > mHistListSize)
        mFuncHistoryList.removeFirst();
}

void MainWindow::setStatusLabel(QString statusText)
{
    ui->lblAppStatus->setText(statusText);
}

void MainWindow::setError(int curError, QString funcText)
{
    QString errStr, str;

    errStr = "Error" + str.setNum((int)curError) + "_";
    mFuncHistoryList.append(errStr + funcText);
    showHistory();
}

void MainWindow::showPlot(bool showIt)
{
    ChildWindow *curChild;

    curChild = activeMdiChild();
    if (curChild) {
        curChild->setShowPlot(showIt);
    }
}

void MainWindow::showHistory()
{
    errDlg.setModal(true);
    errDlg.showHistory(mFuncHistoryList);
    errDlg.exec();
    if (errDlg.historyCleared())
        mFuncHistoryList.clear();
    mHistListSize = errDlg.getHistSize();
    for (int i = mFuncHistoryList.size(); i > mHistListSize; i--)
        mFuncHistoryList.removeFirst();
    errDlg.setHistoryCleared(false);
}

void MainWindow::readWindowPosition()
{
    QSettings windowSettings("Measurement Computing", "Qt Hat Test Linux");
    QVariant autoConnect;

    windowSettings.beginGroup("mainwindow");

    autoConnect = windowSettings.value("autoconnect", true);
    restoreGeometry(windowSettings.value("geometry", saveGeometry()).toByteArray());
    restoreState(windowSettings.value("savestate", saveState()).toByteArray());
    move(windowSettings.value("pos", pos()).toPoint());
    resize(windowSettings.value("size", size()).toSize());
    if (windowSettings.value("maximized", isMaximized()).toBool())
        showMaximized();

    windowSettings.endGroup();
    mAutoConnect = autoConnect.toBool();
    ui->chkAutoDetect->setChecked(mAutoConnect);
}

void MainWindow::writeWindowPosition()
{
    QSettings windowSettings("Measurement Computing", "Qt Hat Test Linux");

    mAutoConnect = ui->chkAutoDetect->isChecked();
    windowSettings.beginGroup("mainwindow");

    windowSettings.setValue("autoconnect", mAutoConnect);
    windowSettings.setValue("geometry", saveGeometry());
    windowSettings.setValue("savestate", saveState());
    windowSettings.setValue("maximized", isMaximized());
    if (!isMaximized()) {
        windowSettings.setValue("pos", pos());
        windowSettings.setValue("size", size());
    }
    windowSettings.endGroup();
}

