#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "childwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->cmdDiscover, SIGNAL(clicked(bool)), this, SLOT(createDiscChild()));
    connect(ui->cmdConfig, SIGNAL(clicked(bool)), this, SLOT(createInfoChild()));
    connect(ui->cmdAIn, SIGNAL(clicked(bool)), this, SLOT(createAiChild()));
    connect(ui->cmdHistory, SIGNAL(clicked(bool)), this, SLOT(showHistory()));
    connect(ui->actionVolts_vs_Time, SIGNAL(triggered(bool)), this, SLOT(showPlot(bool)));
    connect(ui->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(setBoardMenuSelect(QMdiSubWindow*)));

    readWindowPosition();

    functionGroup = new QActionGroup(this);

    QAction *funcAction;
    funcAction = ui->menuFunction->addAction("AIn");
    funcAction->setCheckable(true);
    funcAction->setChecked(true);
    funcAction->setData(UL_AIN);
    functionGroup->addAction(funcAction);
    funcAction = ui->menuFunction->addAction("AInScan");
    funcAction->setCheckable(true);
    funcAction->setData(UL_AINSCAN);
    functionGroup->addAction(funcAction);
    //functionGroup->actions().clear();

    trigTypeGroup = new QActionGroup(this);
    trigTypeGroup->addAction(ui->actionRISING_EDGE);
    trigTypeGroup->addAction(ui->actionFALLING_EDGE);
    trigTypeGroup->addAction(ui->actionACTIVE_HIGH);
    trigTypeGroup->addAction(ui->actionACTIVE_LOW);

    ui->actionRISING_EDGE->setData(TRIG_RISING_EDGE);
    ui->actionFALLING_EDGE->setData(TRIG_FALLING_EDGE);
    ui->actionACTIVE_HIGH->setData(TRIG_ACTIVE_HIGH);
    ui->actionACTIVE_LOW->setData(TRIG_ACTIVE_LOW);
    connect(trigTypeGroup, SIGNAL(triggered(QAction*)), this, SLOT(changeTrigType()));

    ui->actionNOCALIBRATEDATA->setData(OPTS_NOCALIBRATEDATA);
    ui->actionNOSCALEDATA->setData(OPTS_NOSCALEDATA);
    ui->actionCONTINUOUS->setData(OPTS_CONTINUOUS);
    ui->actionEXTCLOCK->setData(OPTS_EXTCLOCK);
    ui->actionEXTTRIGGER->setData(OPTS_EXTTRIGGER);

    connect(functionGroup, SIGNAL(triggered(QAction*)), this, SLOT(curFunctionChanged()));

    connect(ui->actionCONTINUOUS, SIGNAL(triggered(bool)), this, SLOT(curOptionChanged()));
    connect(ui->actionEXTCLOCK, SIGNAL(triggered(bool)), this, SLOT(curOptionChanged()));
    connect(ui->actionEXTTRIGGER, SIGNAL(triggered(bool)), this, SLOT(curOptionChanged()));
    connect(ui->actionNOCALIBRATEDATA, SIGNAL(triggered(bool)), this, SLOT(curOptionChanged()));
    connect(ui->actionNOSCALEDATA, SIGNAL(triggered(bool)), this, SLOT(curOptionChanged()));

    //for(int i = 0; i < MAX_NUMBER_HATS; i++)
    //    mHatList[i] = "";
    mHatList.clear();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    int response;
    uint8_t address;

    foreach (address, mHatList.keys()) {
        if(mcc118_is_open(address))
            response = mcc118_close(address);
    }
    writeWindowPosition();
    event->accept();
}

ChildWindow *MainWindow::activeMdiChild() const
{
    if (QMdiSubWindow *activeSubWindow = ui->mdiArea->activeSubWindow())
        return qobject_cast<ChildWindow *>(activeSubWindow);
    return 0;
}

void MainWindow::createDiscChild()
{
    curFunctionGroupName = "Discovery Functions";
    createChild(FUNC_GROUP_DISC, UL_DISC);
}

void MainWindow::createInfoChild()
{
    curFunctionGroupName = "Hat Info";
    createChild(FUNC_GROUP_MISC, UL_GET_INFO);
}

void MainWindow::createAiChild()
{
    curFunctionGroupName = "Analog Input";
    createChild(FUNC_GROUP_AIN, UL_AIN);
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

    ChildWindow *childWindow = new ChildWindow(ui->mdiArea, utFuncGroup);
    childWindow->setWindowState(Qt::WindowMaximized);
    childWindow->setWindowState(Qt::WindowActive);
    childWindow->show();
    childWindow->setDevAddress(mCurAddress);
    childWindow->setDevName(mCurBoardName);
    childWindow->setCurFunction(defaultFunction);
}

void MainWindow::setBoardMenuSelect(QMdiSubWindow * childWind)
{
    bool optionVisible;
    bool plotShowing;
    int curFunc;
    QString curBoard;
    uint8_t curAddress;
    QString actionName;
    ChildWindow *curChild;

    optionVisible = false;
    plotShowing = false;
    //curChild = activeMdiChild();
    curChild = qobject_cast<ChildWindow *>(childWind);
    ui->lblAppStatus->clear();
    if (curChild) {
        curBoard = curChild->devName();
        if (curBoard == "") {
            ui->lblAppStatus->setText(curFunctionGroupName + ": No device name obtained");
            return;
        }
        curAddress = curChild->devAddress();
        //QString tempString;
        //createFuncMenus();
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
        mScanOptions = curChild->scanOptions();
        switch (curFuncGroup) {
        case FUNC_GROUP_AIN:
            curFunctionGroupName = "Analog Input";
            optionVisible = true;
            switch (curFunc) {
            case UL_AIN:
                actionName = "AIn";
                break;
            case UL_AINSCAN:
                actionName = "AInScan";
                break;
            default:
                actionName = "AIn";
                break;
            }
            break;
        case FUNC_GROUP_MISC:
            curFunctionGroupName = "Miscellaneous Functions";
            optionVisible = false;
            switch (curFunc) {
            default:
                break;
            }
            break;
        case FUNC_GROUP_DISC:
            curFunctionGroupName = "Discovery Functions";
            optionVisible = false;
            break;
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
                uint32_t curMenuVal = (uint32_t)(ioMode->data().toLongLong());
                //tempString += QString("%1, %2;").arg(curMenuVal).arg(childOption);
                //ui->lblAppStatus->setText(tempString);
                if(ioMode->text() != "BACKGROUND")
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
}

void MainWindow::setSelectedDevice()
{
    ChildWindow *mdiChild = activeMdiChild();

    QString function, displayIDString;

    foreach (QAction *devAct, ui->menuBoards->actions()) {
        devAct->setChecked(false);
    }
    QObject *devSelected = this->sender();
    QAction *actDevSelected = qobject_cast<QAction *>(devSelected);
    devSelected->setProperty("checked", true);
    QVariant addressVar = actDevSelected->data();
    mCurAddress = addressVar.toUInt();
    mCurBoardName = mHatList.value(mCurAddress);
//    for (int i = 0; i < MAX_NUMBER_HATS; i++) {
//        if (hatList.v == mCurAddress) {
//            descriptorIndex = i;
//            break;
//        }
//    }
    //devHandle = devList.value(curUniqueID); // deviceHandle.toInt();
    //curBoardName = devDescriptors[descriptorIndex].productName;
    //curUniqueID = devDescriptors[descriptorIndex].uniqueId;

    if(mdiChild) {
        //function = mdiChild->currentFunction();
        mdiChild->setDevAddress(mCurAddress);
        mdiChild->setDevName(mCurBoardName);
        ui->lblAppStatus->setText(curFunctionGroupName + ": " + mCurBoardName);

        //mdiChild->refreshBoardParams();
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
        //curChild->setTmrEnabled(false);
    }
    //mCurFunction = functionGroup->checkedAction()->data().toInt();
}

void MainWindow::curOptionChanged()
{
    ChildWindow *curChild = activeMdiChild();

    QAction *menuOption = (QAction *)this->sender();
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

uint32_t MainWindow::getSoMask(uint32_t optSelected)
{
    uint32_t allOpts = (uint32_t)(
                OPTS_CONTINUOUS | OPTS_EXTCLOCK |
                OPTS_EXTTRIGGER | OPTS_NOCALIBRATEDATA | OPTS_NOSCALEDATA);
    uint32_t maskVal = (uint32_t)(allOpts ^ optSelected);
    return maskVal;
}

void MainWindow::changeTrigType()
{
    ChildWindow *curChild = activeMdiChild();

    QVariant trigSetVal = trigTypeGroup->checkedAction()->data();
    mTriggerType = (TriggerMode)trigSetVal.toLongLong();

    if (curChild)
        curChild->setTriggerType(mTriggerType);
}

void MainWindow::addDeviceToMenu(QString devName, uint8_t devAddress)
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
    connect(newAction, SIGNAL(triggered(bool)), this, SLOT(setSelectedDevice()));
    mHatList.insert(devAddress, devName);

}

void MainWindow::removeDeviceFromMenu(uint8_t devAddress)
{
    bool wasChecked;

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
}

void MainWindow::addFunction(QString funcString)
{
    mFuncHistoryList.append(funcString);
    if (mFuncHistoryList.size() > mHistListSize)
        mFuncHistoryList.removeFirst();
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
    //QVariant autoConnect;

    windowSettings.beginGroup("mainwindow");

    //autoConnect = windowSettings.value("autoconnect", true);
    restoreGeometry(windowSettings.value("geometry", saveGeometry()).toByteArray());
    restoreState(windowSettings.value("savestate", saveState()).toByteArray());
    move(windowSettings.value("pos", pos()).toPoint());
    resize(windowSettings.value("size", size()).toSize());
    if (windowSettings.value("maximized", isMaximized()).toBool())
        showMaximized();

    windowSettings.endGroup();
    //mAutoConnect = autoConnect.toBool();
}

void MainWindow::writeWindowPosition()
{
    QSettings windowSettings("Measurement Computing", "Qt Hat Test Linux");

    //mAutoConnect = ui->chkAutoDetect->isChecked();
    windowSettings.beginGroup("mainwindow");

    //windowSettings.setValue("autoconnect", mAutoConnect);
    windowSettings.setValue("geometry", saveGeometry());
    windowSettings.setValue("savestate", saveState());
    windowSettings.setValue("maximized", isMaximized());
    if (!isMaximized()) {
        windowSettings.setValue("pos", pos());
        windowSettings.setValue("size", size());
    }
    windowSettings.endGroup();
}

