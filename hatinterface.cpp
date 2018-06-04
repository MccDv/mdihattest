#include "hatinterface.h"

HatInterface::HatInterface()
{
    mMainWindow = getMainWindow();
}

MainWindow* HatInterface::getMainWindow()
{
    foreach (QWidget *w, QApplication::topLevelWidgets())
        if (QMainWindow* mainWin = qobject_cast<QMainWindow*>(w))
            return qobject_cast<MainWindow *>(mainWin);
    return nullptr;
}

int HatInterface::openDevice(uint16_t devType, uint8_t address)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;

    funcArgs = "(mAddress)\n";
    switch (devType) {
    case HAT_ID_MCC_118:
        nameOfFunc = "118: Open";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_open(address);
        break;
    case 323:
        //to do: change to constant HAT_ID_MCC_134
        nameOfFunc = "134: Open";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc134_open(address);
        break;
    default:
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QString("(%1)").arg(address);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    if (mResponse != RESULT_SUCCESS) {
        mMainWindow->setError(mResponse, sStartTime + funcStr);
    } else {
        mMainWindow->addFunction(sStartTime + funcStr);
    }

    return mResponse;
}

int HatInterface::closeDevice(uint16_t devType, uint8_t address)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;

    funcArgs = "(mAddress)\n";
    switch (devType) {
    case HAT_ID_MCC_118:
        nameOfFunc = "118: Close";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_close(address);
        break;
    case 323:
        //to do: change to constant HAT_ID_MCC_134
        nameOfFunc = "134: Close";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc134_close(address);
        break;
    default:
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QString("(%1)").arg(address);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    if (mResponse != RESULT_SUCCESS) {
        mMainWindow->setError(mResponse, sStartTime + funcStr);
    } else {
        mMainWindow->addFunction(sStartTime + funcStr);
    }

    return mResponse;
}

bool HatInterface::deviceIsOpen(uint16_t devType, uint8_t address)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    int isOpen;

    funcArgs = "(address) = result\n";
    switch (devType) {
    case HAT_ID_MCC_118:
        nameOfFunc = "118: IsOpen";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        isOpen = mcc118_is_open(address);
        break;
    case 323:
        //to do: change to constant
        nameOfFunc = "134: IsOpen";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        isOpen = mcc134_is_open(address);
        break;
    default:
        isOpen = 0;
        break;
    }

    argVals = QStringLiteral("(%1) = %2")
                .arg(address).arg(isOpen);
    mStatusString = nameOfFunc + argVals;

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    mMainWindow->addFunction(sStartTime + funcStr);
    return isOpen != 0;
}

int HatInterface::getFirmwareVersion(uint16_t devType, uint8_t address, uint16_t& fwMain, uint16_t& fwBoot)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    uint16_t version, boot;

    funcArgs = "(address, &version, &boot)\n";
    version = 0;
    boot = 0;
    switch (devType) {
    case HAT_ID_MCC_118:
        nameOfFunc = "118: firmwareVer";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_firmware_version(address, &version, &boot);
        break;
    case 323:
        //to do: change to constant
        nameOfFunc = "134: firmwareVer ???**???";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        //mResponse = mcc134_firmware_version(mAddress, &version, &boot);
        break;
    default:
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QString("(%1, %2.%3, %4.%5)")
            .arg(address)
            .arg(version >> 8, 1, 16)
            .arg((uint8_t)version, 2, 16, QChar('0'))
            .arg(boot >> 8, 1, 16)
            .arg((uint8_t)boot, 2, 16, QChar('0'));
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    if (mResponse!=RESULT_SUCCESS) {
        mMainWindow->setError(mResponse, sStartTime + funcStr);
    } else {
        mMainWindow->addFunction(sStartTime + funcStr);
        fwMain = version;
        fwBoot = boot;
    }
    return mResponse;
}

int HatInterface::getSerialNumber(uint16_t devType, uint8_t address, QString &serialNum)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    char serNum[10];

    serialNum = "";
    funcArgs = "(mAddress, serNum)\n";
    switch (devType) {
    case HAT_ID_MCC_118:
        nameOfFunc = "118: serialNum";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_serial(address, serNum);
        break;
    case 323:
        //to do: change to constant
        nameOfFunc = "134: serialNum";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc134_serial(address, serNum);
        break;
    default:
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }

    argVals = QString("(%1, %2)")
            .arg(address)
            .arg(serNum);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    if (mResponse!=RESULT_SUCCESS) {
        mMainWindow->setError(mResponse, sStartTime + funcStr);
    } else {
        mMainWindow->addFunction(sStartTime + funcStr);
        serialNum = QString("%1").arg(serNum);
    }
    return mResponse;
}

int HatInterface::stopAInScan(uint16_t devType, uint8_t address)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;

    funcArgs = "(mAddress)\n";
    switch (devType) {
    case HAT_ID_MCC_118:
        nameOfFunc = "118: AInScanStop";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_a_in_scan_stop(address);
        break;
    default:
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QString("(%1)").arg(address);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    if (mResponse!=RESULT_SUCCESS) {
        mMainWindow->setError(mResponse, sStartTime + funcStr);
    } else {
        mMainWindow->addFunction(sStartTime + funcStr);
    }
    return mResponse;
}

int HatInterface::aInScanCleanup(uint16_t devType, uint8_t address)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;

    funcArgs = "(address)\n";
    switch (devType) {
    case HAT_ID_MCC_118:
        nameOfFunc = "118: AInScanCleanup";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_a_in_scan_cleanup(address);
    default:
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QStringLiteral("(%1)").arg(address);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    if (mResponse!=RESULT_SUCCESS) {
        mMainWindow->setError(mResponse, sStartTime + funcStr);
    } else {
        mMainWindow->addFunction(sStartTime + funcStr);
    }
    return mResponse;
}
