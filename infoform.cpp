#include "infoform.h"
#include "ui_infoform.h"
#include "mainwindow.h"

InfoForm::InfoForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InfoForm)
{
    ui->setupUi(this);

    this->setWindowTitle("Information");
    hatInterface = new HatInterface();
    ui->teShowValues->setFont(QFont ("Courier", 8));
    ui->teShowValues->setStyleSheet("QTextEdit { background-color : white; color : blue; }" );
    ui->lblStatus->setStyleSheet("QLabel { color : blue; }" );
    ui->lblInfo->setStyleSheet("QLabel { color : blue; }" );
    mUtFunction = UL_FLASH_LED;

#ifdef HAT_03
    ui->cmbTcType->addItem("TC_TYPE_J", TC_TYPE_J);
    ui->cmbTcType->addItem("TC_TYPE_K", TC_TYPE_K);
    ui->cmbTcType->addItem("TC_TYPE_T", TC_TYPE_T);
    ui->cmbTcType->addItem("TC_TYPE_E", TC_TYPE_E);
    ui->cmbTcType->addItem("TC_TYPE_R", TC_TYPE_R);
    ui->cmbTcType->addItem("TC_TYPE_S", TC_TYPE_S);
    ui->cmbTcType->addItem("TC_TYPE_B", TC_TYPE_B);
    ui->cmbTcType->addItem("TC_TYPE_N", TC_TYPE_N);
    ui->cmbTcType->addItem("Disabled", TC_DISABLED);
#endif

    ui->cmbUtilFunc->addItem("Flash LED", UL_FLASH_LED);
    ui->cmbUtilFunc->addItem("Get Err Msg", UL_GET_ERR_MSG);
    ui->cmbUtilFunc->addItem("Scan Status", UL_GET_STATUS);
    ui->cmbUtilFunc->addItem("AinScan Params", UL_AI_PARAMS);
    ui->cmbUtilFunc->addItem("Calibration", UL_AI_INFO);
    ui->cmbUtilFunc->addItem("Thermocouples", UL_TEMP_INFO);
    ui->cmbUtilFunc->addItem("Trig/Clock Test", UL_TEST);
    ui->cmbUtilFunc->addItem("IEPE Config", UL_IEPE);
    ui->cmbUtilFunc->addItem("Clean scan", UL_SCAN_CLEAN);
    ui->cmbUtilFunc->addItem("Interrupt State", UL_DIO_INFO);

    connect(ui->cmdSysInfo, SIGNAL(clicked(bool)), this, SLOT(showSysInfo()));
    connect(ui->cmbDevList, SIGNAL(currentIndexChanged(int)), this, SLOT(devSelectedChanged()));
    connect(ui->cmdDevParams, SIGNAL(clicked(bool)), this, SLOT(showBoardParameters()));
    connect(ui->cmdFindHats, SIGNAL(clicked(bool)), this, SLOT(findHats()));
    connect(ui->cmdReadCal, SIGNAL(clicked(bool)), this, SLOT(readCalClicked()));
    connect(ui->cmdLoadCal, SIGNAL(clicked(bool)), this, SLOT(loadCalClicked()));
    //connect(ui->cmdFlashLED, SIGNAL(clicked(bool)), this, SLOT(flashLEDClicked()));
    connect(ui->cmdCleanup, SIGNAL(clicked(bool)), this, SLOT(runCleanScan()));
    connect(ui->cmbUtilFunc, SIGNAL(currentIndexChanged(int)), this, SLOT(functionChanged(int)));
    findHats();
}

InfoForm::~InfoForm()
{
    delete ui;
}

void InfoForm::updateParameters()
{
    //ChildWindow *parentWindow;

    //parentWindow = qobject_cast<ChildWindow *>(this->parent());
    //mTInPrefs = parentWindow->tInPrefs();
    //mSerNum = parentWindow->serNum();
    //ui->lblInfo->setText(mSerNum + " Preferences = " + mTInPrefs);

}

void InfoForm::showQueueConfig()
{

}

void InfoForm::findHats()
{
    QString devName;
    uint8_t hatAddress;

    ui->cmbDevList->clear();
    mHatList.clear();
    mHatIDList.clear();
    mHatList = hatInterface->refreshHatList();
    mHatIDList = hatInterface->refreshHatIDList();
    //mHatList = mMainWindowInf->hatList();
    //mHatIDList = mMainWindowInf->hatIDList();
    foreach (hatAddress, mHatList.keys()) {
        ui->cmbDevList->addItem(mHatList.value(hatAddress));
        ui->teShowValues->append(QString("Hat Name; %1  HatID: %2")
                                 .arg(mHatList.value(hatAddress))
                                 .arg(mHatIDList.value(hatAddress)));
    }
    ui->cmbDevList->addItem("MCC118");
    ui->cmbDevList->addItem("MCC134");
    ui->cmbDevList->addItem("MCC152");
    delay(200);
    devSelectedChanged();
}

void InfoForm::devSelectedChanged()
{
    mAddress = -1;
    mDevName = ui->cmbDevList->currentText();
    if(mDevName.contains("[")) {
        foreach (uint8_t keyVal, mHatList.keys()) {
            if(mHatList.value(keyVal) == mDevName) {
                mAddress = keyVal;
                break;
            }
        }
        mHatID = mHatIDList.value(mAddress);
    } else
        mHatID = getHatIdFromName(mDevName);
    this->setWindowTitle("Information " + mDevName);
}

void InfoForm::readCalClicked()
{
    switch (mUtFunction) {
    case UL_GET_STATUS:
        mSelectedFunction = READ_STATUS;
        break;
    case UL_AI_PARAMS:
        mSelectedFunction = READ_SCAN_PARAMS;
        break;
    case UL_AI_INFO:
        mSelectedFunction = READ_CAL;
        break;
    case UL_TEMP_INFO:
        mSelectedFunction = READ_TC_TYPES;
        break;
    /*case UL_TEST:
        mSelectedFunction = NUM_SCAN_CHANS;
        break;*/
    case UL_IEPE:
        mSelectedFunction = READ_IEPE_CONFIG;
        break;
    default:
        break;
    }
    runSelectedFunction();
}

void InfoForm::loadCalClicked()
{
    switch (mUtFunction) {
    case UL_FLASH_LED:
        mSelectedFunction = FLASH_LED;
        break;
    case UL_GET_ERR_MSG:
        mSelectedFunction = READ_ERROR;
        break;
    case UL_GET_STATUS:
        mSelectedFunction = READ_STATUS;
        break;
    case UL_AI_INFO:
        mSelectedFunction = WRITE_CAL;
        break;
    case UL_TEMP_INFO:
        mSelectedFunction = WRITE_TC_TYPE;
        break;
    case UL_TEST:
        mSelectedFunction = CLOCK_TEST;
        break;
    case UL_SCAN_CLEAN:
        mSelectedFunction = WRITE_SCAN_CLEAN;
        break;
    case UL_IEPE:
        mSelectedFunction = WRITE_IEPE_CONFIG;
        break;
    case UL_DIO_INFO:
        mSelectedFunction = READ_INT_STAT;
        break;
    case UL_AI_PARAMS:
        mSelectedFunction = WRITE_SCAN_PARAMS;
        break;
    default:
        break;
    }
    runSelectedFunction();
}

void InfoForm::runSelectedFunction()
{
    switch (mSelectedFunction) {
    case READ_CAL:
        readCal();
        break;
    case WRITE_CAL:
        writeCal();
        break;
    case READ_TC_TYPES:
        readTcTypes();
        break;
    case WRITE_TC_TYPE:
        writeTcType();
        break;
    case CLOCK_TEST:
        readClkTrg();
        break;
    case NUM_SCAN_CHANS:
        readNumScanChans();
        break;
    case FLASH_LED:
        flashLED();
        break;
    case READ_STATUS:
        readStatus();
        break;
    case READ_ERROR:
        showErrorMessage();
        break;
    case READ_INT_STAT:
        readIntStatus();
        break;
    case READ_SCAN_PARAMS:
        readScanParams();
        break;
    case WRITE_SCAN_CLEAN:
        runCleanScan();
        break;
    case READ_IEPE_CONFIG:
        readIEPEConfig();
        break;
    case WRITE_IEPE_CONFIG:
        writeIEPEConfig();
        break;
    case WRITE_SCAN_PARAMS:
        writeScanParams();
        break;
    default:
        break;
    }
}

void InfoForm::functionChanged(int utFunction)
{
    QString spnToolTip, dblOneToolTip;
    QString dblTwoToolTip, flashText;
    int lowLimit, spinVal;
    bool calVisible, readVisible, dblOneVisible;
    bool tcTypeVisible, writeVisible, spinVisible;
    bool flashVisible, scanCleanVisible;

    (void)utFunction;
    ui->cmbTcType->clear();

    mUtFunction = ui->cmbUtilFunc->currentData().toInt();
    calVisible = false;
    spinVisible = true;
    readVisible = true;
    writeVisible = true;
    dblOneVisible = false;
    flashVisible = false;
    scanCleanVisible = false;
    tcTypeVisible = false;
    lowLimit = 0;
    spinVal = 0;
    flashText = "Power: 0=off, 1=on";
    //flashCmdText = "Interrupt State";

    switch (mUtFunction) {
    case UL_FLASH_LED:
        readVisible = false;
        spnToolTip = "Number of flashes";
        break;
    case UL_GET_ERR_MSG:
        spnToolTip = "Result code";
        readVisible = false;
        //flashVisible = true;
        lowLimit = -12;
        break;
    case UL_GET_STATUS:
        scanCleanVisible = true;
        writeVisible = false;
        spinVisible = false;
        break;
    case UL_AI_PARAMS:
#ifdef HAT_05
        tcTypeVisible = true;
        ui->cmbTcType->addItem("Local", SOURCE_LOCAL);
        ui->cmbTcType->addItem("Master", SOURCE_MASTER);
        ui->cmbTcType->addItem("Slave", SOURCE_SLAVE);
#endif
        scanCleanVisible = true;
        spinVisible = true;
        dblOneVisible = true;
        spnToolTip = "Channel count";
        dblOneToolTip = "Rate";
        break;
    case UL_AI_INFO:
        scanCleanVisible = true;
        calVisible = true;
        spnToolTip = "Cal channel";
        dblOneToolTip = "Cal Slope";
        dblTwoToolTip = "Cal Offset";
        break;
    case UL_TEMP_INFO:
#ifdef HAT_03
        ui->cmbTcType->addItem("TC_TYPE_J", TC_TYPE_J);
        ui->cmbTcType->addItem("TC_TYPE_K", TC_TYPE_K);
        ui->cmbTcType->addItem("TC_TYPE_T", TC_TYPE_T);
        ui->cmbTcType->addItem("TC_TYPE_E", TC_TYPE_E);
        ui->cmbTcType->addItem("TC_TYPE_R", TC_TYPE_R);
        ui->cmbTcType->addItem("TC_TYPE_S", TC_TYPE_S);
        ui->cmbTcType->addItem("TC_TYPE_B", TC_TYPE_B);
        ui->cmbTcType->addItem("TC_TYPE_N", TC_TYPE_N);
        ui->cmbTcType->addItem("Disabled", TC_DISABLED);
        readStoredTypes();
#endif
        spnToolTip = "TC channel (-1 loads saved configuration)";
        calVisible = false;
        tcTypeVisible = true;
        lowLimit = -1;
        break;
    case UL_TEST:
        readVisible = false;
        spinVisible = false;
        tcTypeVisible = true;
        ui->cmbTcType->addItem("Clock In", 0);
        ui->cmbTcType->addItem("Clock Low", 1);
        ui->cmbTcType->addItem("Clock High", 2);
        ui->cmbTcType->addItem("Clock 1kHz", 3);
        break;
    case UL_IEPE:
        flashVisible = true;
        lowLimit = -1;
        break;
    case UL_SCAN_CLEAN:
        scanCleanVisible = true;
        spinVisible = false;
        readVisible = false;
        break;
    case UL_DIO_INFO:
        break;
    default:
        break;
    }
    ui->leOffset->setVisible(calVisible);
    ui->leSlope->setVisible(calVisible | dblOneVisible);
    ui->cmdCleanup->setVisible(scanCleanVisible);
    ui->spnCalChan->setVisible(spinVisible);
    ui->cmdReadCal->setVisible(readVisible);
    ui->cmdLoadCal->setVisible(writeVisible);
    ui->cmbTcType->setVisible(tcTypeVisible);
    //ui->cmdFlashLED->setVisible(false);
    ui->leFlashCount->setVisible(flashVisible);

    ui->spnCalChan->setMinimum(lowLimit);
    ui->spnCalChan->setValue(spinVal);

    ui->spnCalChan->setToolTip(spnToolTip);
    ui->leSlope->setToolTip(dblOneToolTip);
    ui->leOffset->setToolTip(dblTwoToolTip);
    ui->leFlashCount->setToolTip(flashText);
}

void InfoForm::showPlotWindow(bool showIt)
{
    mShowPlot = showIt;
}

void InfoForm::showSysInfo()
{
    QFile sysFile;
    QString fileText, tmpText;

    ui->teShowValues->clear();
    system("uname -n > /tmp/node.txt");
    sysFile.setFileName("/tmp/node.txt");
    if(sysFile.open(QIODevice::ReadOnly)) {
        QTextStream in(&sysFile);
        tmpText = in.readLine();
        //tmpText.truncate(fileText.length()-1);
        sysFile.close();
    }

    sysFile.setFileName("/sys/firmware/devicetree/base/model");
    if(sysFile.open(QIODevice::ReadOnly)) {
        QTextStream in(&sysFile);
        fileText = in.readAll();
        fileText.truncate(fileText.length()-1);
        fileText = tmpText + " " + fileText;
        sysFile.close();
    }
    ui->teShowValues->setText(fileText + "\n");

    sysFile.setFileName("/proc/cpuinfo");
    fileText = "";
    if(sysFile.open(QIODevice::ReadOnly)) {
        QTextStream in(&sysFile);
        do {
            tmpText = in.readLine();
            if(tmpText.contains("model name")) {
                int pos = tmpText.indexOf(":") + 2;
                fileText.append(tmpText.mid(pos) + "\n");
            }
            if(tmpText.startsWith("Hardware")) {
                int pos = tmpText.indexOf(":") + 2;
                fileText.append("\nHW " + tmpText.mid(pos) + "\n");
            }
            if(tmpText.startsWith("Revision")) {
                int pos = tmpText.indexOf(":") + 2;
                fileText.append("HW rev " + tmpText.mid(pos) + "\n");
            }
            if(tmpText.startsWith("Serial")) {
                int pos = tmpText.indexOf(":") + 2;
                fileText.append("SerNo " + tmpText.mid(pos) + "\n");
            }
        } while (!in.atEnd());
        sysFile.close();
    }
    ui->teShowValues->append(fileText + "\n");

    sysFile.setFileName("/etc/os-release");
    if(sysFile.open(QIODevice::ReadOnly)) {
        QTextStream in(&sysFile);
        do {
            fileText = in.readLine();
            if(fileText.startsWith("PRETTY")) {
                fileText.remove("PRETTY_NAME=\"");
                break;
            }
        } while (!in.atEnd());
        fileText.truncate(fileText.length()-1);
        sysFile.close();
    }
    ui->teShowValues->append(fileText);

    system("/opt/vc/bin/vcgencmd version > /tmp/vcgenVer.txt");
    system("strings /boot/start.elf | grep VC_BUILD_ID_VERSION > /tmp/elfBoot.txt");
    system("zcat /usr/share/doc/raspberrypi-bootloader/changelog.Debian.gz | head > /tmp/chgLog.txt");
    sysFile.setFileName("/tmp/vcgenVer.txt");
    fileText = "";
    if(sysFile.open(QIODevice::ReadOnly)) {
        QTextStream in(&sysFile);
        fileText = in.readAll();
        fileText.truncate(fileText.length()-1);
        sysFile.close();
    }
    ui->teShowValues->append(fileText + "\n");
    sysFile.setFileName("/tmp/elfBoot.txt");
    fileText = "";
    if(sysFile.open(QIODevice::ReadOnly)) {
        QTextStream in(&sysFile);
        fileText = in.readAll();
        fileText.truncate(fileText.length()-1);
        sysFile.close();
    }
    ui->teShowValues->append(fileText + "\n");
    sysFile.setFileName("/tmp/chgLog.txt");
    fileText = "";
    if(sysFile.open(QIODevice::ReadOnly)) {
        QTextStream in(&sysFile);
        fileText = in.readAll();
        fileText.truncate(fileText.length()-1);
        sysFile.close();
    }
    ui->teShowValues->append(fileText);
}

void InfoForm::showErrorMessage()
{
    QString message;
    int result;

    result = ui->spnCalChan->value();
    message = hatInterface->getErrorMessage(result);
    ui->lblInfo->setText(hatInterface->getStatus());
    ui->teShowValues->setText(message);
}

void InfoForm::flashLED()
{
    uint8_t flashCount;

    flashCount = ui->spnCalChan->text().toUInt();

    hatInterface->blinkLED(mHatID, mAddress, flashCount);
    ui->lblInfo->setText(hatInterface->getStatus());
}

void InfoForm::readStatus()
{
    uint16_t status;
    QString statText;
    uint32_t samplesAvailable = 0;

    mResponse = hatInterface->readAInScanStatus(mHatID, mAddress, status, samplesAvailable);
    ui->lblStatus->setText(hatInterface->getStatus());
    if(mResponse == RESULT_SUCCESS) {
        statText = getStatusText(status);
        //ui->teShowValues->setText("Scan status: " + statText);
        ui->teShowValues->setText(QString("Scan status: %1 with %2 samples to read")
                                 .arg(statText)
                                 .arg(samplesAvailable));
    }
}

void InfoForm::readIntStatus()
{
    int intState;

    intState = hatInterface->getInterruptState();
    ui->lblInfo->setText(hatInterface->getStatus());
    ui->teShowValues->setText(QString("Interrupt state: %1").arg(intState));
}

void InfoForm::readScanParams()
{
    QString sourceName, mcc172Args;
    uint8_t chanCount, source, sync;
    uint32_t bufferSize;
    double rateReturned;

    int numChans = 0;
    mcc172Args = "";

    numChans = hatInterface->aInScanChanCount(mHatID, mAddress);
    if(numChans)
        ui->teShowValues->setText(QString("Device at address %1 has %2 chans in scan.")
                              .arg(mAddress).arg(numChans));
    else {
        ui->teShowValues->setText(QString("Device at address %1 reports 0 chans in scan.")
                              .arg(mAddress));
        ui->teShowValues->append("Is scan thread active?");
    }
    if(numChans > 0)
        ui->spnCalChan->setValue(numChans);

    mResponse = hatInterface->getBufferSize(mHatID, mAddress, bufferSize);
    ui->lblStatus->setText(hatInterface->getStatus());
    if(mResponse == RESULT_SUCCESS) {
        ui->teShowValues->append(QString("\nBuffer size: %1").arg(bufferSize));
    } else {
        QString errText;
        errText = getErrorDescription(mResponse);
        ui->teShowValues->append("\n\ngetBufferSize() returned error " + errText);
    }

    chanCount = ui->spnCalChan->value();
    rateReturned = ui->leSlope->text().toDouble();
    mResponse = hatInterface->getAInScanParameters(mHatID, mAddress, chanCount, source, rateReturned, sync);
    ui->lblStatus->setText(hatInterface->getStatus());
    sourceName = getSourceText(source);
    if(mResponse == RESULT_SUCCESS) {
        if (mHatID == 0x0145) //172
            mcc172Args = QString(", source = %2, sync = %3").arg(sourceName).arg(sync);
        ui->teShowValues->append(QString("\nActual scan rate: %1")
                                 .arg(rateReturned) + mcc172Args);
        ui->leSlope->setText(QString("%1").arg(rateReturned));
    } else {
        QString errText;
        errText = getErrorDescription(mResponse);
        ui->teShowValues->append("\n\ngetAInScanParameters() returned error " + errText);
    }
}

void InfoForm::writeScanParams()
{
    QString sourceName;
    uint8_t source;
    double rate;

    rate = ui->leSlope->text().toDouble();
    source = ui->cmbTcType->currentData().toUInt();
    mResponse = hatInterface->ainClockConfigWrite(mHatID, mAddress, source, rate);
    ui->lblStatus->setText(hatInterface->getStatus());
    sourceName = getSourceText(source);
    if(mResponse == RESULT_SUCCESS) {
        ui->teShowValues->setText(QString("\nScan rate set to: %1 (if 172, source set to %2)")
                                  .arg(rate)
                                  .arg(sourceName));
    } else {
        QString errText;
        errText = getErrorDescription(mResponse);
        ui->teShowValues->setText("\n\nsetAInScanParameters() returned error " + errText);
    }
}

void InfoForm::readIEPEConfig()
{
    uint8_t channel, value;
    uint8_t firstChan, lastChan;
    int chanSelected;

    chanSelected = ui->spnCalChan->value();
    value = -1; //power: 0=off, 1=on
    ui->teShowValues->setText("IEPE settings:\n\n");
    if(chanSelected == -1) {
        firstChan = 0;
        lastChan = 1;
    } else {
        firstChan = chanSelected;
        lastChan = chanSelected;
    }
    for(channel = firstChan; channel <= lastChan; channel++) {
        mResponse = hatInterface->iepeConfigRead(mHatID, mAddress, channel, value);
        ui->lblStatus->setText(hatInterface->getStatus());
        if(mResponse == RESULT_SUCCESS) {
            ui->teShowValues->append(QString("Value read from channel %1: %2)")
                                      .arg(channel)
                                      .arg(value));
        }
    }
}

void InfoForm::writeIEPEConfig()
{
    uint8_t channel, value;
    uint8_t firstChan, lastChan;
    int chanSelected;

    chanSelected = ui->spnCalChan->value();
    value = ui->leFlashCount->text().toInt(); //power: 0=off, 1=on
    ui->teShowValues->setText("IEPE settings:\n\n");
    if(chanSelected == -1) {
        firstChan = 0;
        lastChan = 1;
    } else {
        firstChan = chanSelected;
        lastChan = chanSelected;
    }
    for(channel = firstChan; channel <= lastChan; channel++) {
        mResponse = hatInterface->iepeConfigWrite(mHatID, mAddress, channel, value);
        ui->lblStatus->setText(hatInterface->getStatus());
        if(mResponse == RESULT_SUCCESS) {
            ui->teShowValues->append(QString("Value written to channel %1: %2)")
                                      .arg(channel)
                                      .arg(value));
        }
    }
}

void InfoForm::readClkTrg()
{
    uint8_t clockState, mode, trigState;

    mode = ui->cmbTcType->currentData().toUInt();
    mResponse = hatInterface->testClock(mHatID, mAddress, mode, clockState);
    mResponse = hatInterface->testTrig(mHatID, mAddress, trigState);

    ui->teShowValues->setText(QString("Device at address %1 clock test response:").arg(mAddress));
    ui->teShowValues->append(QString("Mode: %1").arg(ui->cmbTcType->currentText()));
    ui->teShowValues->append(QString("Clock state: %1").arg(clockState));
    ui->teShowValues->append(QString("Trigger state: %1").arg(trigState));
}

void InfoForm::readNumScanChans()
{
    int numChans;

    numChans = hatInterface->aInScanChanCount(mHatID, mAddress);
    if(numChans)
        ui->teShowValues->setText(QString("Device at address %1 has %2 chans in scan.")
                              .arg(mAddress).arg(numChans));
    else {
        ui->teShowValues->setText(QString("Device at address %1 reports 0 chans in scan.")
                              .arg(mAddress));
        ui->teShowValues->append("Is scan thread active?");
    }
    ui->spnCalChan->setValue(numChans);
}

void InfoForm::runCleanScan()
{
    mResponse = hatInterface->aInScanCleanup(mHatID, mAddress);
    ui->lblInfo->setText(hatInterface->getStatus());
}

void InfoForm::readCal()
{
    QString dataText;
    uint8_t chan, curChan;
    int numChans;
    double slope, offset;
    QString calDate;

    curChan = ui->spnCalChan->value();
    ui->teShowValues->clear();

    mResponse = hatInterface->readCalDate(mHatID, mAddress, calDate);
    ui->lblInfo->setText(hatInterface->getStatus());

    if(mResponse == RESULT_SUCCESS) {
        dataText = "<style> th, td { padding-right: 10px;}</style><tr>";
        dataText.append(QString("<td>Cal date: %1</td></tr><tr>").arg(calDate));
        dataText.append("</tr><tr>");
    }

    numChans = hatInterface->getNumAInChans(mHatID);

    for(chan = 0; chan < numChans; chan++) {
        mResponse = hatInterface->readCalCoeffs(mHatID, mAddress, chan, slope, offset);
        ui->lblInfo->setText(hatInterface->getStatus());
        /*mResponse = mcc118_calibration_coefficient_read(mAddress, chan, &slope, &offset);
        argVals = QStringLiteral("(%1, %2, %3, %4)")
                .arg(mAddress)
                .arg(chan)
                .arg(slope)
                .arg(offset);
        if(mResponse != RESULT_SUCCESS)
            break;*/
        dataText.append(QString("<td>Chan: %1</td><td>Slope:  %2</td><td>Offset: %3</td>")
                        .arg(chan).arg(slope).arg(offset));
        dataText.append("</tr><tr>");
        if(chan == curChan) {
            ui->leSlope->setText(QString("%1").arg(slope));
            ui->leOffset->setText(QString("%1").arg(offset));
        }
    }
    ui->teShowValues->setHtml(dataText);
    ui->lblInfo->setText(hatInterface->getStatus());
}

void InfoForm::writeCal()
{
    uint8_t chan;
    double slope, offset;

    chan = ui->spnCalChan->value();
    slope = ui->leSlope->text().toDouble();
    offset = ui->leOffset->text().toDouble();
    ui->teShowValues->clear();
    hatInterface->writeCalCoeffs(mHatID, mAddress, chan, slope, offset);
    delay(300);
    readCal();
}

void InfoForm::showBoardParameters()
{
    bool isOpen;
    int numChans, prec;
    int32_t aInMinCode, aInMaxCode;
    double aInMaxVolts, aInMinVolts;
    double aInMaxRange, aInMinRange;

    ui->lblInfo->clear();
    ui->lblStatus->clear();
    ui->teShowValues->clear();
    prec = 6;
    if(mHatID == HAT_ID_MCC_172)
        prec = 12;
    if(mHatID == HAT_ID_MCC_134)
        prec = 9;
    if(mHatID == 0) {
        ui->teShowValues->setText(QString("%1 device is not supported in this software version.")
                                  .arg(mDevName));
        return;
    }
    ui->teShowValues->setText(QString("Device %1").arg(mDevName));
    ui->teShowValues->append(QString("Address: %1").arg(mAddress));

    isOpen = hatInterface->deviceIsOpen(mHatID, mAddress);
    ui->lblStatus->setText(hatInterface->getStatus());
    if(!isOpen) {
        ui->teShowValues->setText(QString("%1 device is not open.\n").arg(mDevName)
             + "Use Discover to open device.\n");
    }
    numChans = hatInterface->getNumAInChans(mHatID);
    ui->teShowValues->append(QString("AIn chans: %1").arg(numChans));
    if((mHatID == HAT_ID_MCC_118) | (mHatID == HAT_ID_MCC_172) | (mHatID == HAT_ID_MCC_134)) {
        aInMinCode = hatInterface->getAInCodeMin(mHatID);
        aInMaxCode = hatInterface->getAInCodeMax(mHatID);
        ui->teShowValues->append(QString("Code range: %1 to %2")
                                 .arg(aInMinCode)
                                 .arg(aInMaxCode));
        aInMinRange = hatInterface->getAInRangeMin(mHatID);
        aInMaxRange = hatInterface->getAInRangeMax(mHatID);
        ui->teShowValues->append(QString("Range: %1 to %2")
                                 .arg(aInMinRange)
                                 .arg(aInMaxRange));
        aInMinVolts = hatInterface->getAInVoltsMin(mHatID);
        aInMaxVolts = hatInterface->getAInVoltsMax(mHatID);
        ui->teShowValues->append(QString("Volts: %1 to %2")
                                 .arg(aInMinVolts, 0, 'f', prec)
                                 .arg(aInMaxVolts, 0, 'f', prec));
    }

#ifdef HAT_04
    numChans = hatInterface->getNumAOutChans(mHatID);
    ui->teShowValues->append(QString("AOut chans: %1").arg(numChans));
    int lowCount = hatInterface->getAOutCodeMin(mHatID);
    int highCount = hatInterface->getAOutCodeMax(mHatID);
    ui->teShowValues->append(QString("AOut code range: %1 to %2")
                             .arg(lowCount)
                             .arg(highCount));
    double lowVolts = hatInterface->getAOutVoltsMin(mHatID);
    double highVolts = hatInterface->getAOutVoltsMax(mHatID);
    ui->teShowValues->append(QString("AOut voltage min/max: %1 to %2")
                             .arg(lowVolts)
                             .arg(highVolts));
    double lowRange = hatInterface->getAOutRangeMin(mHatID);
    double highRange = hatInterface->getAOutRangeMax(mHatID);
    ui->teShowValues->append(QString("AOut range min/max: %1 to %2")
                             .arg(lowRange)
                             .arg(highRange));
    int numDioChans = hatInterface->getNumDioChans(mHatID);
    ui->teShowValues->append(QString("Dio chans: %1").arg(numDioChans));
#endif

    if (isOpen) {
        ui->lblInfo->setText(QString("Device at address %1 is ready").arg(mAddress));
        if(mHatID == HAT_ID_MCC_118) {
            numChans = hatInterface->aInScanChanCount(mHatID, mAddress);
            ui->teShowValues->append(QString("Scan channels: %1").arg(numChans));
        }
#ifdef HAT_03
        double temp;
        if(mHatID == HAT_ID_MCC_134) {
            mResponse = hatInterface->boardTemp(mHatID, mAddress, 0, temp);
            ui->lblStatus->setText(hatInterface->getStatus());
            ui->teShowValues->append(QString("CJC temperature: %1ᵒC").arg(temp));
        }
#endif
    }
}

void InfoForm::readStoredTypes()
{
    QSettings windowSettings("Measurement Computing", "Qt Hat Test Linux");
    QVariant storedTInPrefs;
    QString serNum;

    mResponse = hatInterface->getSerialNumber(mHatID, mAddress, serNum);
    ui->lblInfo->setText(hatInterface->getStatus());

    if(mResponse == RESULT_SUCCESS) {
        mSerNum = QString("%1").arg(serNum);
    }

    windowSettings.beginGroup(mSerNum);
    storedTInPrefs = windowSettings.value("TempPrefs", "0,0,0,0");
    mTInPrefs = storedTInPrefs.toString();
    windowSettings.endGroup();
    if(mTInPrefs == "")
        mTInPrefs = "255,255,255,255";
}

void InfoForm::readTcTypes()
{
    QString dataText, typeName;
    uint8_t chan, curChan;
    int numChans;
    uint8_t tcType, typeIndex;

    curChan = ui->spnCalChan->value();
    ui->teShowValues->clear();

    numChans = hatInterface->getNumAInChans(mHatID);

    for(chan = 0; chan < numChans; chan++) {
        mResponse = hatInterface->readTcTypes(mHatID, mAddress, chan, tcType);
        ui->lblInfo->setText(hatInterface->getStatus());
        typeName = getTcTypeName(tcType);
        dataText.append(QString("<td>Chan: %1</td><td>TC Type:  %2 (%3)</td>")
                        .arg(chan).arg(tcType).arg(typeName));
        dataText.append("</tr><tr>");
        if(chan == curChan) {
            if(tcType == TC_DISABLED)
                typeIndex = TC_TYPE_N + 1;
            else
                typeIndex = tcType;
            ui->cmbTcType->setCurrentIndex(typeIndex);
        }
    }
    ui->teShowValues->setHtml(dataText);
    ui->lblInfo->setText(hatInterface->getStatus());
}

void InfoForm::writeTcType()
{
    uint8_t chan, tcType;
    QString tInPrefs, typeString;
    QVector<uint8_t> typeList;

    ui->teShowValues->clear();
    chan = ui->spnCalChan->value();
    tInPrefs = "";
    //when spinner is -1, result is 255 (uint8)
    typeList = getTcTypesFromString(mTInPrefs);
    if(chan > 15) {
        for (int curChan = 0; curChan < typeList.length(); curChan++) {
            tcType = typeList.at(curChan);
            mResponse = hatInterface->writeTcType(mHatID, mAddress, curChan, tcType);
            if (mResponse != RESULT_SUCCESS) {
                tInPrefs = "";
                break;
            } else {
                typeString = getTcTypeName(tcType);
                tInPrefs += typeString + ",";
            }
        }
    } else {
        tcType = ui->cmbTcType->currentData().toUInt();
        mResponse = hatInterface->writeTcType(mHatID, mAddress, chan, tcType);
        uint8_t storedType;
        storedType = typeList.at(chan);
        if(tcType != storedType) {
            mTInPrefs = setTcTypeInString(mTInPrefs, chan, tcType);
            ui->lblStatus->setText("New prefs: " + mTInPrefs + "for mSerNum " + mSerNum);
        }
    }
    delay(300);
    if (tInPrefs.length() > 0) {
        tInPrefs.chop(1);
        ui->lblStatus->setText("Preference string: " + tInPrefs);
    }
    readTcTypes();
}


