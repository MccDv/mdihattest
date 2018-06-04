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

QHash<uint8_t, QString> HatInterface::refreshHatList()
{
    mHatList = mMainWindow->hatList();
    return mHatList;
}

QHash<uint8_t, uint16_t>  HatInterface::refreshHatIDList()
{
    mHatIDList = mMainWindow->hatIDList();
    return mHatIDList;
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
    return (isOpen != 0);
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

int HatInterface::blinkLED(uint16_t devType, uint8_t address, uint8_t count)
{
    QString nameOfFunc, funcArgs, funcStr;
    QString argVals;
    QTime t;
    QString sStartTime;

    funcArgs = "(address, flashCount)\n";
    switch (devType) {
    case HAT_ID_MCC_118:
        nameOfFunc = "118: BlinkLED";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_blink_led(address, count);
        break;
    case 323:
        //to do: change to constant HAT_ID_MCC_134
        nameOfFunc = "134: BlinkLED ???**???";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        //mResponse = mcc134_blink_led(address, count);
        mResponse = RESULT_INVALID_DEVICE;
        break;
    default:
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QString("(%1, %2)")
            .arg(address)
            .arg(count);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    if (mResponse!=RESULT_SUCCESS) {
        mMainWindow->setError(mResponse, sStartTime + funcStr);
    } else {
        mMainWindow->addFunction(sStartTime + funcStr);
    }
    return mResponse;
}

int HatInterface::readCalDate(uint16_t devType, uint8_t address, QString &calDate)
{
    QString nameOfFunc, funcArgs, funcStr;
    QString argVals;
    QTime t;
    QString sStartTime;
    char dateReturned[12];

    funcArgs = "(address, calDate)\n";
    switch (devType) {
    case HAT_ID_MCC_118:
        nameOfFunc = "118: CalDate";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_calibration_date(address, dateReturned);
        break;
    case 323:
        //to do: change to constant HAT_ID_MCC_134
        nameOfFunc = "134: CalDate";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc134_calibration_date(address, dateReturned);
        break;
    default:
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QStringLiteral("(%1, %2)")
            .arg(address)
            .arg(dateReturned);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    if (mResponse!=RESULT_SUCCESS) {
        mMainWindow->setError(mResponse, sStartTime + funcStr);
    } else {
        mMainWindow->addFunction(sStartTime + funcStr);
        calDate = QString("%1").arg(dateReturned);
    }
    return mResponse;
}

int HatInterface::getNumAInChans(uint16_t devType)
{
    QString nameOfFunc, funcArgs, funcStr;
    QString argVals;
    QTime t;
    QString sStartTime;
    int numChans;

    funcArgs = "() = result\n";
    switch (devType) {
    case HAT_ID_MCC_118:
        nameOfFunc = "118: numAIChans";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        numChans = mcc118_a_in_num_channels();
        break;
    case 323:
        //to do: change to constant HAT_ID_MCC_134
        nameOfFunc = "134: numAIChans";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        numChans = mcc134_a_in_num_channels();
        break;
    default:
        numChans = 0;
        break;
    }
    argVals = "()";
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(numChans);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals + QString(" = %1").arg(numChans);
    mMainWindow->addFunction(sStartTime + funcStr);

    return numChans;
}

int HatInterface::readCalCoeffs(uint16_t devType, uint8_t address, uint8_t chan, double &slope, double &offset)
{
    QString nameOfFunc, funcArgs, funcStr;
    QString argVals;
    QTime t;
    QString sStartTime;
    double chanSlope, chanOffset;

    funcArgs = "() = result\n";
    switch (devType) {
    case HAT_ID_MCC_118:
        nameOfFunc = "118: readCalCoeffs";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_calibration_coefficient_read(address, chan, &chanSlope, &chanOffset);
        break;
    case 323:
        //to do: change to constant HAT_ID_MCC_134
        nameOfFunc = "134: readCalCoeffs";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc134_calibration_coefficient_read(address, chan, &chanSlope, &chanOffset);
        break;
    default:
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    if (mResponse!=RESULT_SUCCESS) {
        mMainWindow->setError(mResponse, sStartTime + funcStr);
    } else {
        mMainWindow->addFunction(sStartTime + funcStr);
    }
    slope = chanSlope;
    offset = chanOffset;
    return mResponse;
}

int HatInterface::aInRead(uint16_t devType, uint8_t address, uint8_t chan, uint32_t options, double &value)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    double data;

    funcArgs = "(mAddress, curChan, mScanOptions, &data)\n";
    switch (devType) {
    case HAT_ID_MCC_118:
        nameOfFunc = "118: AInRead";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_a_in_read(address, chan, options, &data);
        break;
    case 323:
        //to do: change to constant HAT_ID_MCC_134
        nameOfFunc = "134: AInRead";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc134_a_in_read(address, chan, options, &data);
        //mResponse = RESULT_INVALID_DEVICE;
        break;
    default:
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QStringLiteral("(%1, %2, %3, %4)")
            .arg(address)
            .arg(chan)
            .arg(options)
            .arg(data);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    if (mResponse!=RESULT_SUCCESS) {
        mMainWindow->setError(mResponse, sStartTime + funcStr);
    } else {
        mMainWindow->addFunction(sStartTime + funcStr);
    }
    value = data;
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

void HatInterface::addToMenu(uint16_t devType, uint8_t address, QString devName)
{
    mMainWindow->addDeviceToMenu(devName, address, devType);
}

void HatInterface::removeFromMenu(uint8_t address)
{
    mMainWindow->removeDeviceFromMenu(address);
}
