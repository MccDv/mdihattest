#include "hatdiscovery.h"
#include "ui_hatdiscovery.h"
#include "mainwindow.h"
#include "childwindow.h"

//MainWindow* getMainWindow();
//MainWindow *mMainWindowHd;

HatDiscovery::HatDiscovery(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HatDiscovery)
{
    ui->setupUi(this);
    hatInterface = new HatInterface();
    //mMainWindowHd = getMainWindow();
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

void HatDiscovery::showQueueConfig()
{

}

void HatDiscovery::functionChanged(int utFunction)
{
    mUtFunction = utFunction;
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
    bool isOpen;
    uint8_t address;
    uint16_t version, boot;
    uint16_t devId;
    QString serNum;
    bool hat134Support;

    hat134Support = true;

#ifndef HAT_03
    hat134Support = false;
#endif

    version = 0;
    boot = 0;
    ui->lblInfo->clear();
    ui->lblStatus->clear();
    ui->textEdit->clear();
    address = hatInfoList[mDevIndex].address;
    ui->textEdit->setText(QString("Index %1").arg(mDevIndex));
    ui->textEdit->append(QString("Address: %1").arg(address));
    ui->textEdit->append(QString("ID: %1").arg(hatInfoList[mDevIndex].id));
    ui->textEdit->append(QString("Version: %1\n").arg(hatInfoList[mDevIndex].version));

    devId = hatInfoList[mDevIndex].id;
    switch (devId) {
    case 323:
        if(!hat134Support) {
            ui->textEdit->append("This software version does not support this device");
            return;
        }
        break;
    default:
        break;
    }
    isOpen = hatInterface->deviceIsOpen(devId, address);
    ui->lblInfo->setText(hatInterface->getStatus());

    if (!isOpen) {
        ui->lblStatus->setText(QString("Device at address %1 is not open").arg(address));
        mAddress = -1;
        return;
    } else {
        mAddress = address;
        ui->lblStatus->setText(QString("Device at address %1 is ready").arg(address));

        mResponse = hatInterface->getFirmwareVersion(devId, mAddress, version, boot);
        ui->lblInfo->setText(hatInterface->getStatus());

        if(mResponse == RESULT_SUCCESS) {
            ui->textEdit->append(QString("Version %1.%2").arg((version) >> 8, 1, 16)
                                 .arg((uint8_t)version, 2, 16, QChar('0')));
            ui->textEdit->append(QString("Boot %1.%2").arg(boot >> 8, 1, 16, QChar('0'))
                                 .arg((uint8_t)boot, 2, 16, QChar('0')));
        }

        mResponse = hatInterface->getSerialNumber(devId, mAddress, serNum);
        ui->lblInfo->setText(hatInterface->getStatus());

        if(mResponse == RESULT_SUCCESS) {
            ui->textEdit->append(QString("Serial num:  %1").arg(serNum));
        }
    }
}

void HatDiscovery::showPlotWindow(bool showIt)
{
    mShowPlot = showIt;
}

void HatDiscovery::runOpenDevice(uint8_t address)
{
    QString devName;
    uint16_t devId;

    devId = hatInfoList[mDevIndex].id;
    mResponse = hatInterface->openDevice(devId, address);
    ui->lblInfo->setText(hatInterface->getStatus());

    if (mResponse == RESULT_SUCCESS) {
        mAddress = address;
        devName = QString(hatInfoList[mDevIndex].product_name).left(7)
                + QString(" [%1]").arg(mAddress);
        hatInterface->addToMenu(devId, mAddress, devName);
        showBoardParameters();
    }
}

void HatDiscovery::runCloseDevice(uint8_t address)
{
    uint16_t devId;

    devId = hatInfoList[mDevIndex].id;
    mResponse = hatInterface->closeDevice(devId, address);
    ui->lblInfo->setText(hatInterface->getStatus());

    if (mResponse == RESULT_SUCCESS) {
        hatInterface->removeFromMenu(address);
        showBoardParameters();
    }
}
