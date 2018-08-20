#include "miscform.h"
#include "ui_miscform.h"

MiscForm::MiscForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MiscForm)
{
    ui->setupUi(this);
    hatInterface = new HatInterface();
    tmrGoTimer = new QTimer(this);
    mHistListSize = 50;

    mUseTimer = false;
    mGoTmrIsRunning = false;
    ui->cmdStop->setVisible(mUseTimer);

    ui->teStatus->setFont(QFont ("Courier", 8));
    ui->teStatus->setStyleSheet("QTextEdit { background-color : white; color : blue; }" );
    ui->lblStatus->setStyleSheet("QLabel { color : blue; } ");
    ui->lblInfo->setStyleSheet("QLabel { color : blue; } ");

    connect(ui->cmdStart, SIGNAL(clicked(bool)), this, SLOT(onClickCmdGo()));
    connect(tmrGoTimer, SIGNAL(timeout()), this, SLOT(runSelectedFunction()));
    connect(ui->cmdStop, SIGNAL(clicked(bool)), this, SLOT(onClickCmdStop()));
    connect(this, SIGNAL(callbackTriggered()), this, SLOT(onCallback()));
    connect(ui->cmdDisableEvent, SIGNAL(clicked(bool)), this, SLOT(runEventDisable()));
    connect(ui->cmdEnableEvent, SIGNAL(clicked(bool)), this, SLOT(runEventEnable()));

    initDevices();
}

MiscForm::~MiscForm()
{
    delete ui;
}

void MiscForm::keyPressEvent(QKeyEvent *event)
{
    int keyCode = event->key();
    if (keyCode == Qt::Key_Escape) {
        toggleGoTimer(false);
    }
}

void MiscForm::closeEvent(QCloseEvent *event)
{
    event->accept();
}

void MiscForm::updateParameters()
{
    ChildWindow *parentWindow;

    parentWindow = qobject_cast<ChildWindow *>(this->parent());
    mCurGroup = parentWindow->curFunctionGroup();

    mUseTimer = parentWindow->tmrEnabled();
    mTmrInterval = parentWindow->tmrInterval();
    mOneSampPerForTotalSamps = parentWindow->tmrSampPerInterval();

}

void MiscForm::initDevices()
{
    uint8_t hatAddress;

    mHatList.clear();
    mHatIDList.clear();
    mHatList = hatInterface->refreshHatList();
    mHatIDList = hatInterface->refreshHatIDList();
    ui->teStatus->setText("Current devices:\n\n");
    foreach (hatAddress, mHatList.keys()) {
        ui->teStatus->append(QString("  Hat Name; %1  HatID: %2")
                                 .arg(mHatList.value(hatAddress))
                                 .arg(mHatIDList.value(hatAddress)));
    }
}

void MiscForm::functionChanged(int utFunction)
{
    (void)utFunction;
}

void MiscForm::groupChanged(int newGroup)
{
    (void)newGroup;
}

void MiscForm::onClickCmdGo()
{
    runSelectedFunction();
    toggleGoTimer(mUseTimer);
}

void MiscForm::onClickCmdStop()
{
    toggleGoTimer(false);
}

void MiscForm::runSelectedFunction()
{
    QFont goFont = ui->cmdStart->font();
    ui->teStatus->clear();
    getInterruptStatus();
    if(mGoTmrIsRunning) {
        goFont.setBold(!ui->cmdStart->font().bold());
        ui->cmdStart->setFont(goFont);
    }
}

void MiscForm::toggleGoTimer(bool enableTimer)
{
    ChildWindow *parentWindow;
    QFont goFont = ui->cmdStart->font();

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
    ui->cmdStart->setFont(goFont);
}

void MiscForm::showPlotWindow(bool showIt)
{
    (void)showIt;
}

void MiscForm::showQueueConfig()
{

}

void MiscForm::waitForInterupt()
{

}

void MiscForm::getInterruptStatus()
{
    int intState;
    uint8_t hatAddress, value;
    uint16_t hatID;
    bool readOnInt;
    QString statString;

    readOnInt = ui->chkRead->isChecked();
    intState = hatInterface->getInterruptState();
    ui->teStatus->append(QString("Interrupt state: %1\n\n").arg(intState));
    ui->lblStatus->setText(hatInterface->getStatus());
    if(intState == 0) {
        ui->teStatus->append("No devices generating interrupt.");
    } else {
        foreach (hatAddress, mHatIDList.keys()) {
            hatID = mHatIDList.value(hatAddress);
            if(hatID == HAT_ID_MCC_152) {
                mResponse = hatInterface->dioIntStatusPort(hatID, hatAddress, value);
                if(value != 0) {
                    ui->teStatus->append(QString("MCC 152 [%1] interrupt request: %2")
                                     .arg(hatAddress)
                                     .arg(value));
                    if(readOnInt) {
                        mResponse = hatInterface->dioInPortRead(hatID, hatAddress, value);
                        statString = hatInterface->getStatus();
                        ui->teStatus->append(QString("Value read: %1 (").arg(value) + statString + ")");
                    }
                }
            }
        }
    }
}

void MiscForm::runEventEnable()
{
    callbackFunction callbackFunc = &MiscForm::eventCallback;
    mResponse = hatInterface->enableCallback(callbackFunc, this);
    ui->lblStatus->setText(hatInterface->getStatus());
}

void MiscForm::eventCallback(void *userData)
{
    MiscForm *miscForm = (MiscForm *)userData;
    emit miscForm->callbackTriggered();
}

void MiscForm::onCallback()
{
    QTime t;
    QString sStartTime;

    sStartTime = t.currentTime().toString("hh:mm:ss.zzz");
    ui->teStatus->setText("Callback called at " + sStartTime);
    getInterruptStatus();
}

void MiscForm::runEventDisable()
{
    mResponse = hatInterface->disableCallback();
    ui->lblStatus->setText(hatInterface->getStatus());
}
