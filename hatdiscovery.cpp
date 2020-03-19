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

    ui->cmbFilter->addItem("All Hats", HAT_ID_ANY);
    ui->cmbFilter->addItem("MCC118", HAT_ID_MCC_118);
#ifdef HAT_03
    ui->cmbFilter->addItem("MCC134", HAT_ID_MCC_134);
#endif
#ifdef HAT_04
    ui->cmbFilter->addItem("MCC152", HAT_ID_MCC_152);
#endif
#ifdef HAT_05
    ui->cmbFilter->addItem("MCC172", HAT_ID_MCC_172);
#endif
#ifdef HAT_06
    ui->cmbFilter->addItem("MCC128", HAT_ID_MCC_128);
#endif

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
    uint16_t hatFilter;

    hatFilter = ui->cmbFilter->currentData().toUInt();
    ui->listWidget->clear();

    mNumHats = hat_list(hatFilter, hatInfoList);
    for(int hatNum = 0; hatNum < mNumHats; hatNum++) {
        ui->listWidget->addItem(hatInfoList[hatNum].product_name);
    }
    ui->lblStatus->setText(QString("Number of Hats found: %1").arg(mNumHats));
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
    QString devName;
    ChildWindow *parentWindow;

    parentWindow = qobject_cast<ChildWindow *>(this->parent());
    mDevIndex = ui->listWidget->currentIndex().row();
    mAddress = hatInfoList[mDevIndex].address;
    devName = QString(hatInfoList[mDevIndex].product_name).left(7)
                + QString(" [%1]").arg(mAddress);
    parentWindow->setDevName(devName);
    hatInterface->updateAppStatus(FUNC_GROUP_DISC, devName);
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

void HatDiscovery::runSelectedFunction()
{

}

void HatDiscovery::showBoardParameters()
{
    bool isOpen;
    uint8_t address;
    uint16_t version, boot;
    uint16_t devId;
    QString serNum;
    bool hat134Support;
    bool hat152Support;
    bool hat172Support;
    bool hat128Support;

    hat134Support = true;
    hat152Support = true;
    hat172Support = true;
    hat128Support = true;

#ifndef HAT_03
    hat134Support = false;
#endif
#ifndef HAT_04
    hat152Support = false;
#endif
#ifndef HAT_05
    hat172Support = false;
#endif
#ifndef HAT_06
    hat128Support = false;
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
    ui->textEdit->append(QString("HW Version: %1\n").arg(hatInfoList[mDevIndex].version));

    devId = hatInfoList[mDevIndex].id;
    switch (devId) {
    case 323:
        if(!hat134Support) {
            ui->textEdit->append("This software version does not support this device");
            return;
        }
        break;
    case 324:
        if(!hat152Support) {
            ui->textEdit->append("This software version does not support this device");
            return;
        }
        break;
    case 325:
        if(!hat172Support) {
            ui->textEdit->append("This software version does not support this device");
            return;
        }
        break;
    case 326:
        if(!hat128Support) {
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
            ui->textEdit->append(QString("FW Version %1.%2").arg((version) >> 8, 1, 16)
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
