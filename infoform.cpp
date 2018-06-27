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
    mUtFunction = UL_AI_INFO;

#ifdef HAT_03
    ui->cmbTcType->addItem("TC_TYPE_J", TC_TYPE_J);
    ui->cmbTcType->addItem("TC_TYPE_K", TC_TYPE_K);
    ui->cmbTcType->addItem("TC_TYPE_T", TC_TYPE_T);
    ui->cmbTcType->addItem("TC_TYPE_E", TC_TYPE_E);
    ui->cmbTcType->addItem("TC_TYPE_R", TC_TYPE_R);
    ui->cmbTcType->addItem("TC_TYPE_S", TC_TYPE_S);
    ui->cmbTcType->addItem("TC_TYPE_B", TC_TYPE_B);
    ui->cmbTcType->addItem("TC_TYPE_N", TC_TYPE_N);
#endif

    connect(ui->cmdSysInfo, SIGNAL(clicked(bool)), this, SLOT(showSysInfo()));
    connect(ui->cmbDevList, SIGNAL(currentIndexChanged(QString)), this, SLOT(devSelectedChanged(QString)));
    connect(ui->cmdDevParams, SIGNAL(clicked(bool)), this, SLOT(showBoardParameters()));
    connect(ui->cmdFindHats, SIGNAL(clicked(bool)), this, SLOT(findHats()));
    connect(ui->cmdReadCal, SIGNAL(clicked(bool)), this, SLOT(readCalClicked()));
    connect(ui->cmdLoadCal, SIGNAL(clicked(bool)), this, SLOT(loadCalClicked()));
    connect(ui->cmdFlashLED, SIGNAL(clicked(bool)), this, SLOT(flashLEDClicked()));
    connect(ui->cmdCleanup, SIGNAL(clicked(bool)), this, SLOT(cleanScanClicked()));
    findHats();
}

InfoForm::~InfoForm()
{
    delete ui;
}

void InfoForm::updateParameters()
{
    ChildWindow *parentWindow;

    parentWindow = qobject_cast<ChildWindow *>(this->parent());
}

void InfoForm::showQueueConfig()
{

}

void InfoForm::findHats()
{
    QString devName;
    uint8_t hatAddress;

    ui->cmbDevList->clear();
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
    delay(200);
    devName = ui->cmbDevList->currentText();
    devSelectedChanged(devName);
}

void InfoForm::devSelectedChanged(QString devName)
{
    foreach (uint8_t keyVal, mHatList.keys()) {
        if(mHatList.value(keyVal) == devName) {
            mAddress = keyVal;
            mDevName = devName;
            break;
        }
    }
    mHatID = mHatIDList.value(mAddress);
    this->setWindowTitle("Information " + devName);
}

void InfoForm::readCalClicked()
{
    switch (mUtFunction) {
    case UL_AI_INFO:
        mSelectedFunction = READ_CAL;
        break;
    case UL_TEMP_INFO:
        mSelectedFunction = READ_TC_TYPES;
        break;
    case UL_TEST:
        mSelectedFunction = NUM_SCAN_CHANS;
        break;
    default:
        break;
    }
    runSelectedFunction();
}

void InfoForm::loadCalClicked()
{
    switch (mUtFunction) {
    case UL_AI_INFO:
        mSelectedFunction = WRITE_CAL;
        break;
    case UL_TEMP_INFO:
        mSelectedFunction = WRITE_TC_TYPE;
        break;
    case UL_TEST:
        mSelectedFunction = CLOCK_TEST;
        break;
    default:
        break;
    }
    runSelectedFunction();
}

void InfoForm::flashLEDClicked()
{
    switch (mUtFunction) {
    case UL_AI_INFO:
        mSelectedFunction = READ_STATUS;
        break;
    case UL_TEMP_INFO:
        mSelectedFunction = READ_STATUS;
        break;
    case UL_TEST:
        mSelectedFunction = FLASH_LED;
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
#ifdef HAT_03
    case READ_TC_TYPES:
        readTcTypes();
        break;
    case WRITE_TC_TYPE:
        writeTcType();
        break;
#endif
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
    default:
        break;
    }
}

void InfoForm::functionChanged(int utFunction)
{
    QString readCmdText;
    QString writeCmdText, flashCmdText;
    QString spnToolTip;
    bool calVisible, readVisible, scanCleanVisible;
    bool tcTypeVisible, spinVisible;

    ui->cmbTcType->clear();

    mUtFunction = utFunction;
    calVisible = true;
    spinVisible = true;
    readVisible = true;
    scanCleanVisible = true;
    tcTypeVisible = false;
    switch (mUtFunction) {
    case UL_AI_INFO:
        readCmdText = "Read Cal";
        writeCmdText = "Load Cal";
        flashCmdText = "Scan Status";
        spnToolTip = "Cal channel";
        break;
#ifdef HAT_03
    case UL_TEMP_INFO:
        ui->cmbTcType->addItem("TC_TYPE_J", TC_TYPE_J);
        ui->cmbTcType->addItem("TC_TYPE_K", TC_TYPE_K);
        ui->cmbTcType->addItem("TC_TYPE_T", TC_TYPE_T);
        ui->cmbTcType->addItem("TC_TYPE_E", TC_TYPE_E);
        ui->cmbTcType->addItem("TC_TYPE_R", TC_TYPE_R);
        ui->cmbTcType->addItem("TC_TYPE_S", TC_TYPE_S);
        ui->cmbTcType->addItem("TC_TYPE_B", TC_TYPE_B);
        ui->cmbTcType->addItem("TC_TYPE_N", TC_TYPE_N);
        readCmdText = "Read TC types";
        writeCmdText = "Load TC type";
        spnToolTip = "TC channel";
        flashCmdText = "Flash LED";
        scanCleanVisible = false;
        calVisible = false;
        tcTypeVisible = true;
        break;
#endif
    case UL_TEST:
        writeCmdText = "Trig/Clock Test";
        readCmdText = "Num Scan Chans";
        flashCmdText = "Flash LED";
        calVisible = false;
        tcTypeVisible = true;
        spinVisible = false;
        scanCleanVisible = false;
        ui->cmbTcType->addItem("Clock In", 0);
        ui->cmbTcType->addItem("Clock Low", 1);
        ui->cmbTcType->addItem("Clock High", 2);
        ui->cmbTcType->addItem("Clock 1kHz", 3);
        break;
    default:
        break;
    }
    ui->cmdReadCal->setText(readCmdText);
    ui->cmdLoadCal->setText(writeCmdText);
    ui->cmdFlashLED->setText(flashCmdText);
    ui->leOffset->setVisible(calVisible);
    ui->leSlope->setVisible(calVisible);
    ui->cmdCleanup->setVisible(scanCleanVisible);
    ui->spnCalChan->setToolTip(spnToolTip);
    ui->spnCalChan->setVisible(spinVisible);
    ui->cmdReadCal->setVisible(readVisible);
    ui->cmbTcType->setVisible(tcTypeVisible);
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

void InfoForm::flashLED()
{
    uint8_t flashCount;

    flashCount = ui->leFlashCount->text().toUInt();

    hatInterface->blinkLED(mHatID, mAddress, flashCount);
    ui->lblInfo->setText(hatInterface->getStatus());
}

void InfoForm::readStatus()
{
    uint16_t status;
    QString statText;
    uint32_t samplesAvailable;

    mResponse = hatInterface->readAInScanStatus(mHatID, mAddress, status, samplesAvailable);
    ui->lblStatus->setText(hatInterface->getStatus());
    statText = getStatusText(status);
    ui->teShowValues->setText("Scan status: " + statText);
    ui->teShowValues->append(QString("Scan status: %1 with %2 samples to read")
                             .arg(statText)
                             .arg(samplesAvailable));
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

}

void InfoForm::cleanScanClicked()
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

#ifdef HAT_03
void InfoForm::readTcTypes()
{
    QString dataText, typeName;
    uint8_t chan, curChan;
    int numChans;
    uint8_t tcType;

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
        if(chan == curChan)
            ui->cmbTcType->setCurrentIndex(tcType);
    }
    ui->teShowValues->setHtml(dataText);
    ui->lblInfo->setText(hatInterface->getStatus());
}
#endif

#ifdef HAT_03
void InfoForm::writeTcType()
{
    uint8_t chan, tcType;

    chan = ui->spnCalChan->value();
    tcType = ui->cmbTcType->currentData().toUInt();
    ui->teShowValues->clear();
    mResponse = hatInterface->writeTcType(mHatID, mAddress, chan, tcType);
    delay(300);
    readTcTypes();
}
#endif

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
    int numChans;
    double temp;

    ui->lblInfo->clear();
    ui->lblStatus->clear();
    ui->teShowValues->clear();
    ui->teShowValues->setText(QString("Device %1").arg(mDevName));
    ui->teShowValues->append(QString("Address: %1").arg(mAddress));

    isOpen = hatInterface->deviceIsOpen(mHatID, mAddress);
    ui->lblStatus->setText(hatInterface->getStatus());

    if (isOpen) {
        ui->lblInfo->setText(QString("Device at address %1 is ready").arg(mAddress));
        numChans = hatInterface->getNumAInChans(mHatID);
        ui->teShowValues->append(QString("AIn chans: %1").arg(numChans));
        if(mHatID == HAT_ID_MCC_118) {
            numChans = hatInterface->aInScanChanCount(mHatID, mAddress);
            ui->teShowValues->append(QString("Scan channels: %1").arg(numChans));
        }
#ifdef HAT_03
        if(mHatID == HAT_ID_MCC_134) {
            mResponse = hatInterface->boardTemp(mHatID, mAddress, temp);
            ui->lblStatus->setText(hatInterface->getStatus());
            ui->teShowValues->append(QString("CJC temperature: %1ᵒC").arg(temp));
        }
#endif
    } else {
        ui->teShowValues->setText(QString("Device at %1 is not open.\n\n").arg(mAddress)
                 + "Use Discover to open device.");
    }
}
