#include "aout.h"
#include "ui_aout.h"
#include "mainwindow.h"

AOut::AOut(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AOut)
{
    ui->setupUi(this);

    hatInterface = new HatInterface();

    functionGroup = new QActionGroup(this);
    buffer = NULL;

    mAddress = -1;
    mNumHats = 0;
    mScanOptions = 0;
    mAoResolution = 12;
    mCurFunction = UL_AOUT;
    mQueueEnabled = false;
    mOneSampPerForTotalSamps = false;

    ui->teShowValues->setFont(QFont ("Courier", 8));
    ui->teShowValues->setStyleSheet("QTextEdit { background-color : white; color : blue; }" );
    ui->lblStatus->setStyleSheet("QLabel { color : blue; }" );
    ui->lblInfo->setStyleSheet("QLabel { color : blue; }" );
    ui->lblRateReturned->setFont(QFont ("Courier", 8));
    ui->lblRateReturned->setStyleSheet("QLabel { background-color : white; color : blue; }" );

    connect(ui->cmdGo, SIGNAL(clicked(bool)), this, SLOT(goCmdClicked()));
    connect(ui->cmdStop, SIGNAL(clicked(bool)), this, SLOT(stopCmdClicked()));
    connect(ui->hSldAoutVal, SIGNAL(valueChanged(int)), this, SLOT(updateValueBox()));
    connect(ui->leAoutVal, SIGNAL(textChanged(QString)), this, SLOT(valueChanged()));
    initDeviceParams();
}

AOut::~AOut()
{
    delete ui;
}

void AOut::keyPressEvent(QKeyEvent *event)
{
    int keyCode = event->key();
    if (keyCode == Qt::Key_Escape) {
        stopCmdClicked();
    }
    //if (keyCode == Qt::Key_F6)
    //    readBuffer();
}

void AOut::closeEvent(QCloseEvent *event)
{
    event->accept();
}

void AOut::updateParameters()
{
    ChildWindow *parentWindow;
    parentWindow = qobject_cast<ChildWindow *>(this->parent());
    mDevName = parentWindow->devName();
    mAddress = parentWindow->devAddress();
    mHatID = parentWindow->devId();
    mScanOptions = parentWindow->scanOptions();

    mUseTimer = parentWindow->tmrEnabled();
    mStopOnStart = parentWindow->tmrStopOnStart();
    mGoTmrIsRunning = parentWindow->tmrRunning();
    mTmrInterval = parentWindow->tmrInterval();
    mOneSampPerForTotalSamps = parentWindow->tmrSampPerInterval();

    mOptNames = getOptionNames(mScanOptions);
    ui->lblStatus->clear();
    this->setWindowTitle(mFuncName + ": " + mDevName);
}

void AOut::initDeviceParams()
{
    int value;
    int loopCount;


    mNumAoChans = hatInterface->getNumAOutChans(mHatID);
    mMaxCode = hatInterface->getAOutCodeMax(mHatID);
    mMaxVolts = hatInterface->getAOutVoltsMax(mHatID);
    mMinVolts = hatInterface->getAOutVoltsMin(mHatID);

    loopCount = 0;
    if(mMaxCode == 0)
        mMaxCode = 4095;
    do {
        value = qPow(2, loopCount);
        loopCount++;
    } while (value < mMaxCode);
    mAoResolution = loopCount - 1;
    ui->hSldAoutVal->setMaximum(mMaxCode);
}

void AOut::showQueueConfig()
{
    /*
    queueSetup = new QueueDialog(this);
    if (mChanList.count()) {
        queueSetup->setChanList(mChanList);
        queueSetup->setNumQueueElements(mChanList.count());
    }
    connect(queueSetup, SIGNAL(accepted()), this, SLOT(queueDialogResponse()));
    queueSetup->exec();
    */
}

void AOut::functionChanged(int utFunction)
{
    mCurFunction = utFunction;
    this->setUiForFunction();
}

void AOut::setUiForFunction()
{
    bool scanVisible;
    //bool voltCheckVisible;

    mPlot = false;
    scanVisible = false;
    //voltCheckVisible = false;
    switch (mCurFunction) {
    case UL_AOUT:
        mFuncName = "ulAOut";
        ui->leNumSamples->setText("10");
        break;
    case UL_AOUT_SCAN:
        mFuncName = "ulAOutScan";
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
    /*ui->chkVolts->setVisible(voltCheckVisible);
    ui->chkCJC->setVisible(cjcCheckVisible);
    ui->cmdStop->setEnabled(false);*/
    showPlotWindow(mPlot);
    this->setWindowTitle(mFuncName + ": " + mDevName);
}

void AOut::updateValueBox()
{
    //double maxVolts, minVolts;
    //int maxCode;

    //maxCode = hatInterface->getAOutCodeMax(mHatID);
    //maxVolts = hatInterface->getAOutVoltsMax(mHatID);
    //minVolts = hatInterface->getAOutVoltsMin(mHatID);
    if(mMaxVolts == 0)
        mMaxVolts = 4.99878;
    if(mMaxCode == 0)
        mMaxCode = 4095;
    int sliderOut = ui->hSldAoutVal->value();
    if(mScanOptions & OPTS_NOSCALEDATA)
        ui->leAoutVal->setText(QString("%1").arg(sliderOut));
    else {
        double vOutVal = getVoltsFromCounts(mMaxCode, mMinVolts, mMaxVolts, sliderOut);
        //double vOutVal = getVoltsFromResolution(mAoResolution, minVolts, maxVolts, sliderOut);
        ui->leAoutVal->setText(QString("%1").arg(vOutVal));
    }
    //runAOutFunc();
}

void AOut::valueChanged()
{
    runAOutFunc();
}

void AOut::goCmdClicked()
{
    ChildWindow *parentWindow;
    parentWindow = qobject_cast<ChildWindow *>(this->parent());
    bool tmrIsEnabled;

    mTotalRead = 0;
    tmrIsEnabled = parentWindow->tmrEnabled();
    mUseTimer = tmrIsEnabled;
    runSelectedFunction();
}

void AOut::stopCmdClicked()
{
    //stopScan();
    mUseTimer = false;
}

void AOut::runSelectedFunction()
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
    case UL_AOUT:
        runAOutFunc();
        break;
    case UL_AOUT_SCAN:
        runAOutScanFunc();
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

void AOut::showPlotWindow(bool showIt)
{
    QFrame::Shape frameShape;

    mPlot = showIt;

    int curIndex = 0;
    frameShape = QFrame::Box;

    if (showIt) {
        curIndex = 1;
        frameShape = QFrame::NoFrame;
    }
    ui->stackedWidget->setFrameShape(frameShape);
    ui->stackedWidget->setCurrentIndex(curIndex);
    /*if(buffer) {
        if(mPlot)
            plotScan(0, 0, mTotalRead);
        else
            printData(0, 0, mTotalRead);
    }*/
}

void AOut::runAOutFunc()
{
    uint8_t aOutLowChan, aOutHighChan;
    double data;
    double dataArray[2];
    bool useArray;

    useArray = false;
    data = 0.0;
    aOutLowChan = ui->spnLowChan->value();
    aOutHighChan = ui->spnHighChan->value();
    mChanCount = (aOutHighChan - aOutLowChan) + 1;
    data = ui->leAoutVal->text().toDouble();
    for(int i = 0; i < 2; i++)
        dataArray[i] = data;

    uint8_t curChan;
    mRunning = true;
    if(useArray) {
        mResponse = hatInterface->aOutWriteAll(mHatID, mAddress, mScanOptions, *dataArray);
    } else {
        for (curChan = aOutLowChan; curChan <= aOutHighChan; curChan++) {
            mResponse = hatInterface->aOutWrite(mHatID, mAddress, curChan, mScanOptions, data);
            ui->lblInfo->setText(hatInterface->getStatus());
        }
    }
    mRunning = false;
}

void AOut::runAOutScanFunc()
{

}
