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

    intState = hatInterface->getInterruptState();
    ui->teStatus->setText(QString("Interrupt state: %1\n\n").arg(intState));
    ui->lblStatus->setText(hatInterface->getStatus());
    if(intState == 0) {
        ui->teStatus->append("No devices generating interrupt.");
    } else {
        foreach (hatAddress, mHatIDList.keys()) {
            hatID = mHatIDList.value(hatAddress);
            if(hatID == HAT_ID_MCC_152) {
                mResponse = hatInterface->dioIntStatusPort(hatID, hatAddress, value);
                if(value != 0)
                    ui->teStatus->append(QString("MCC 152 [%1] interrupt request: %2")
                                     .arg(hatAddress)
                                     .arg(value));
            }
        }
    }
}
