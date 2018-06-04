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
    hatInterface = new HatInterface();
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
    uint8_t flashCount;

    flashCount = ui->leFlashCount->text().toUInt();

    hatInterface->blinkLED(mHatID, mAddress, flashCount);
    ui->lblInfo->setText(hatInterface->getStatus());
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
    ui->lblInfo->setText(nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse));
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
    bool isOpen;
    int numChans;

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
    } else {
        ui->teShowValues->setText(QString("Device at %1 is not open.\n\n").arg(mAddress)
                 + "Use Discover to open device.");
    }
}
