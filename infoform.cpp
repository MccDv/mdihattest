#include "infoform.h"
#include "ui_infoform.h"
#include "mainwindow.h"

MainWindow* getMainWindow();
MainWindow *mMainWindowInf;

InfoForm::InfoForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InfoForm)
{
    ui->setupUi(this);

    this->setWindowTitle("Information");
    mMainWindowInf = getMainWindow();
    ui->teShowValues->setFont(QFont ("Courier", 8));
    ui->teShowValues->setStyleSheet("QTextEdit { background-color : white; color : blue; }" );
    ui->lblStatus->setStyleSheet("QLabel { color : blue; }" );
    ui->lblInfo->setStyleSheet("QLabel { color : blue; }" );

    connect(ui->cmdSysInfo, SIGNAL(clicked(bool)), this, SLOT(showSysInfo()));
    connect(ui->cmbDevList, SIGNAL(currentIndexChanged(QString)), this, SLOT(devSelectedChanged(QString)));
    connect(ui->cmdDevParams, SIGNAL(clicked(bool)), this, SLOT(showBoardParameters()));
    connect(ui->cmdFindHats, SIGNAL(clicked(bool)), this, SLOT(findHats()));
    connect(ui->cmdReadCal, SIGNAL(clicked(bool)), this, SLOT(readCal()));
    connect(ui->cmdFlashLED, SIGNAL(clicked(bool)), this, SLOT(flashLED()));
    connect(ui->cmdLoadCal, SIGNAL(clicked(bool)), this, SLOT(writeCal()));
}

InfoForm::~InfoForm()
{
    delete ui;
}

void InfoForm::updateParameters()
{

}

void InfoForm::showQueueConfig()
{

}

void InfoForm::findHats()
{
    QString devName;
    uint8_t hatAddress;

    ui->cmbDevList->clear();
    mHatList = mMainWindowInf->hatList();
    mHatIDList = mMainWindowInf->hatIDList();
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

void InfoForm::runSelectedFunction()
{

}

void InfoForm::functionChanged(int utFunction)
{

}

void InfoForm::showPlotWindow(bool showIt)
{

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
    QString nameOfFunc, funcArgs, funcStr;
    QString argVals;
    QTime t;
    QString sStartTime;
    uint8_t flashCount;

    flashCount = ui->leFlashCount->text().toUInt();
    funcArgs = "(address, flashCount) = result\n";
    switch (mHatID) {
    case HAT_ID_MCC_118:
        nameOfFunc = "118: BlinkLED";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_blink_led(mAddress, flashCount);
        break;
    case 323:
        //to do: change to constant HAT_ID_MCC_134
        nameOfFunc = "134: BlinkLED ???**???";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        //mResponse = mcc134_blink_led(mAddress, flashCount);
        break;
    default:
        break;
    }
    argVals = QStringLiteral("(%1, %2)")
            .arg(mAddress)
            .arg(flashCount);
    ui->lblInfo->setText(nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse));

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    if (mResponse!=RESULT_SUCCESS) {
        mMainWindowInf->setError(mResponse, sStartTime + funcStr);
        return;
    } else {
        mMainWindowInf->addFunction(sStartTime + funcStr);
    }
}

void InfoForm::readCal()
{
    QString nameOfFunc, funcArgs, funcStr;
    QString argVals, dataText;
    QTime t;
    QString sStartTime;
    uint8_t chan, curChan;
    int numChans;
    double slope, offset;
    char calDate[12];

    curChan = ui->spnCalChan->value();
    ui->teShowValues->clear();

    funcArgs = "(address, calDate) = result\n";
    switch (mHatID) {
    case HAT_ID_MCC_118:
        nameOfFunc = "118: CalDate";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_calibration_date(mAddress, calDate);
        break;
    case 323:
        //to do: change to constant HAT_ID_MCC_134
        nameOfFunc = "134: BlinkLED ???**???";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc134_calibration_date(mAddress, calDate);
        break;
    default:
        break;
    }
    argVals = QStringLiteral("(%1, %2)")
            .arg(mAddress)
            .arg(calDate);
    ui->lblInfo->setText(nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse));

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    dataText = "<style> th, td { padding-right: 10px;}</style><tr>";
    if (mResponse!=RESULT_SUCCESS) {
        mMainWindowInf->setError(mResponse, sStartTime + funcStr);
        return;
    } else {
        mMainWindowInf->addFunction(sStartTime + funcStr);
        dataText.append(QString("<td>Cal date: %1</td>").arg(calDate));
        dataText.append("</tr><tr>");
    }

    numChans = mcc118_a_in_num_channels();
    nameOfFunc = "118: ReadCal";
    funcArgs = "(address, chan, &slope, &offset) = result\n";
    sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
    dataText.append("</tr><tr>");
    for(chan = 0; chan < numChans; chan++) {
        mResponse = mcc118_calibration_coefficient_read(mAddress, chan, &slope, &offset);
        argVals = QStringLiteral("(%1, %2, %3, %4)")
                .arg(mAddress)
                .arg(chan)
                .arg(slope)
                .arg(offset);
        if(mResponse != RESULT_SUCCESS)
            break;
        dataText.append(QString("<td>Chan: %1</td><td>Slope:  %2</td><td>Offset: %3</td>")
                        .arg(chan).arg(slope).arg(offset));
        dataText.append("</tr><tr>");
        if(chan == curChan) {
            ui->leSlope->setText(QString("%1").arg(slope));
            ui->leOffset->setText(QString("%1").arg(offset));
        }
    }
    ui->teShowValues->setHtml(dataText);
    ui->lblInfo->setText(nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse));

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    if (mResponse!=RESULT_SUCCESS) {
        mMainWindowInf->setError(mResponse, sStartTime + funcStr);
        return;
    } else {
        mMainWindowInf->addFunction(sStartTime + funcStr);
    }
}

void InfoForm::writeCal()
{
    QString nameOfFunc, funcArgs, funcStr;
    QString argVals;
    QTime t;
    QString sStartTime;
    uint8_t chan;
    double slope, offset;

    if(!mcc118_is_open(mAddress)) {
        ui->teShowValues->setText
                ("Device is not open.\n\nUse Discover to open device.");
        return;
    }
    chan = ui->spnCalChan->value();
    slope = ui->leSlope->text().toDouble();
    offset = ui->leOffset->text().toDouble();
    ui->teShowValues->clear();
    nameOfFunc = "118: WriteCal";
    funcArgs = "(address, chan, slope, offset) = result\n";
    sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
    mResponse = mcc118_calibration_coefficient_write(mAddress, chan, slope, offset);
    argVals = QStringLiteral("(%1, %2, %3, %4)")
            .arg(mAddress)
            .arg(chan)
            .arg(slope)
            .arg(offset);
    ui->lblInfo->setText(nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse));

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    if (mResponse!=RESULT_SUCCESS) {
        mMainWindowInf->setError(mResponse, sStartTime + funcStr);
        return;
    } else {
        mMainWindowInf->addFunction(sStartTime + funcStr);
    }
    delay(300);
    readCal();
}

void InfoForm::showBoardParameters()
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    uint8_t address;
    uint16_t version, boot;
    int isOpen;
    char serNum[10];

    ui->lblInfo->clear();
    ui->lblStatus->clear();
    ui->teShowValues->clear();
    ui->teShowValues->setText(QString("Device %1").arg(mDevName));
    ui->teShowValues->append(QString("Address: %1").arg(mAddress));

    isOpen = 0;
    funcArgs = "(address) = result\n";
    switch (mHatID) {
    case HAT_ID_MCC_118:
        nameOfFunc = "118: IsOpen";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        isOpen = mcc118_is_open(mAddress);
        break;
    case 323:
        //to do: change to constant HAT_ID_MCC_134
        nameOfFunc = "134: IsOpen";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        isOpen = mcc134_is_open(mAddress);
        break;
    default:
        break;
    }
    argVals = QStringLiteral("(%1) = %2")
                .arg(address).arg(isOpen);
    ui->lblInfo->setText(nameOfFunc + argVals);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    mMainWindowInf->addFunction(sStartTime + funcStr);
    if (isOpen) {
        //mAddress = address;
        ui->lblStatus->setText(QString("Device at address %1 is ready").arg(mAddress));

        return;
        nameOfFunc = "118: firmwareVer";
        funcArgs = "(mAddress, &version, &boot)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_firmware_version(mAddress, &version, &boot);
        argVals = QString("(%1, %2.%3, %4.%5)")
                .arg(mAddress)
                .arg((uint8_t)version >> 8, 16)
                .arg((uint8_t)version, 16)
                .arg((uint8_t)boot >> 8, 16)
                .arg((uint8_t)boot, 16);
        ui->lblInfo->setText(nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse));

        funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
        if (mResponse!=RESULT_SUCCESS) {
            mMainWindowInf->setError(mResponse, sStartTime + funcStr);
            return;
        } else {
            mMainWindowInf->addFunction(sStartTime + funcStr);
            ui->teShowValues->append(QString("Version %1.%2").arg((version) >> 8, 1, 16).arg((uint8_t)version, 2, 16));
            ui->teShowValues->append(QString("Boot %1.%2").arg(boot >> 8, 1, 16, QChar('0')).arg((uint8_t)boot, 2, 16, QChar('0')));
            //ui->textEdit->append(QString("Version %1").arg(version, 0, 16));
            //ui->textEdit->append(QString("Boot %1").arg(boot, 0, 16));
        }

        nameOfFunc = "118: serialNum";
        funcArgs = "(mAddress, serNum)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_serial(mAddress, serNum);
        argVals = QStringLiteral("(%1, %2)")
                .arg(mAddress)
                .arg(serNum);
        ui->lblInfo->setText(nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse));

        if (mResponse!=RESULT_SUCCESS) {
            mMainWindowInf->setError(mResponse, sStartTime + funcStr);
            return;
        } else {
            mMainWindowInf->addFunction(sStartTime + funcStr);
            ui->teShowValues->append(QString("Serial num:  %1").arg(serNum));
        }
    } else {
        ui->teShowValues->setText
                (nameOfFunc + QString(" reports device at %1 is not open.\n\n").arg(address)
                 + "Use Discover to open device.");
    }
}
