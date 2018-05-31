#include "hatdiscovery.h"
#include "ui_hatdiscovery.h"
#include "mainwindow.h"
#include "childwindow.h"

MainWindow* getMainWindow();
MainWindow *mMainWindowHd;

HatDiscovery::HatDiscovery(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HatDiscovery)
{
    ui->setupUi(this);
    mMainWindowHd = getMainWindow();
    ui->textEdit->setFont(QFont ("Courier", 8));
    ui->textEdit->setStyleSheet("QTextEdit { background-color : white; color : blue; }" );
    ui->lblStatus->setStyleSheet("QLabel { color : blue; }" );
    ui->lblInfo->setStyleSheet("QLabel { color : blue; }" );

    connect(ui->cmdDiscover, SIGNAL(clicked(bool)), this, SLOT(readHatList()));
    connect(ui->cmdOpen, SIGNAL(clicked(bool)), this, SLOT(openCmdClicked()));
    connect(ui->cmdClose, SIGNAL(clicked(bool)), this, SLOT(closeCmdClicked()));
    connect(ui->listWidget, SIGNAL(itemSelectionChanged()), this, SLOT(devSelectedChanged()));
}

HatDiscovery::~HatDiscovery()
{
    delete ui;
}

void HatDiscovery::readHatList()
{
    ui->listWidget->clear();

    mNumHats = hat_list(HAT_ID_ANY, hatInfoList);
    for(int hatNum = 0; hatNum < mNumHats; hatNum++) {
        ui->listWidget->addItem(hatInfoList[hatNum].product_name);
    }
}

void HatDiscovery::updateParameters()
{

}

void HatDiscovery::functionChanged(int utFunction)
{

}

void HatDiscovery::devSelectedChanged()
{
    mDevIndex = ui->listWidget->currentIndex().row();
    showBoardParameters();
}

void HatDiscovery::openCmdClicked()
{
    uint8_t address;

    if(mDevIndex<mNumHats) {
        address = hatInfoList[mDevIndex].address;
        runOpenDevice(address);
    }
}

void HatDiscovery::closeCmdClicked()
{
    uint8_t address;

    if(mDevIndex<mNumHats) {
        address = hatInfoList[mDevIndex].address;
        runCloseDevice(address);
    }
}

void HatDiscovery::showBoardParameters()
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    uint8_t address;
    uint16_t version, boot;
    char serNum[10];

    ui->lblInfo->clear();
    ui->lblStatus->clear();
    ui->textEdit->clear();
    address = hatInfoList[mDevIndex].address;
    ui->textEdit->setText(QString("Index %1").arg(mDevIndex));
    ui->textEdit->append(QString("Address: %1").arg(address));
    ui->textEdit->append(QString("ID: %1").arg(hatInfoList[mDevIndex].id));
    ui->textEdit->append(QString("Version: %1\n").arg(hatInfoList[mDevIndex].version));

    nameOfFunc = "118: IsOpen";
    funcArgs = "(address) = result\n";
    sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
    mResponse = mcc118_is_open(address);
    //mResponse = mcc118_blink_led(address, 2);
    argVals = QStringLiteral("(%1) = %2")
                .arg(address).arg(mResponse);
    ui->lblInfo->setText(nameOfFunc + argVals);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    mMainWindowHd->addFunction(sStartTime + funcStr);
    if (mResponse == 0) {
        ui->lblStatus->setText(QString("Device at address %1 is not open").arg(address));
        mAddress = -1;
        return;
    } else {
        mAddress = address;
        ui->lblStatus->setText(QString("Device at address %1 is ready").arg(address));

        nameOfFunc = "118: firmwareVer";
        funcArgs = "(mAddress, &version, &boot)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_firmware_version(mAddress, &version, &boot);
        argVals = QString("(%1, %2.%3, %4.%5)")
                .arg(mAddress)
                .arg(version >> 8, 1, 16)
                .arg((uint8_t)version, 2, 16, QChar('0'))
                .arg(boot >> 8, 1, 16)
                .arg((uint8_t)boot, 2, 16, QChar('0'));
        ui->lblInfo->setText(nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse));

        funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
        if (mResponse!=RESULT_SUCCESS) {
            mMainWindowHd->setError(mResponse, sStartTime + funcStr);
            return;
        } else {
            mMainWindowHd->addFunction(sStartTime + funcStr);
            ui->textEdit->append(QString("Version %1.%2").arg((version) >> 8, 1, 16).arg((uint8_t)version, 2, 16, QChar('0')));
            ui->textEdit->append(QString("Boot %1.%2").arg(boot >> 8, 1, 16, QChar('0')).arg((uint8_t)boot, 2, 16, QChar('0')));
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
            mMainWindowHd->setError(mResponse, sStartTime + funcStr);
            return;
        } else {
            mMainWindowHd->addFunction(sStartTime + funcStr);
            ui->textEdit->append(QString("Serial num:  %1").arg(serNum));
        }
    }
}

void HatDiscovery::showPlotWindow(bool showIt)
{

}

void HatDiscovery::runOpenDevice(uint8_t address)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime, devName;

    nameOfFunc = "118: Open";
    funcArgs = "(mAddress)\n";
    sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
    mResponse = mcc118_open(address);
    argVals = QStringLiteral("(%1)").arg(address);
    ui->lblInfo->setText(nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse));

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    if (mResponse != RESULT_SUCCESS) {
        mMainWindowHd->setError(mResponse, sStartTime + funcStr);
        mAddress = 0;
        return;
    } else {
        mMainWindowHd->addFunction(sStartTime + funcStr);
        mAddress = address;
        devName = QString(hatInfoList[0].product_name).left(7)
                + QString(" [%1]").arg(mAddress);
        mMainWindowHd->addDeviceToMenu(devName, mAddress);
        showBoardParameters();
    }
}

void HatDiscovery::runCloseDevice(uint8_t address)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;

    nameOfFunc = "118: Close";
    funcArgs = "(mAddress)\n";
    sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
    mResponse = mcc118_close(address);
    argVals = QStringLiteral("(%1)")
                .arg(address);
    ui->lblInfo->setText(nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse));

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    if (mResponse != RESULT_SUCCESS) {
        mMainWindowHd->setError(mResponse, sStartTime + funcStr);
        return;
    } else {
        mMainWindowHd->addFunction(sStartTime + funcStr);
        showBoardParameters();
    }
}
