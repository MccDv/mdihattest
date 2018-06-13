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
    reportResult(mResponse, sStartTime + funcStr);
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
    reportResult(mResponse, sStartTime + funcStr);
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
    argVals = QString("(%1, %2)")
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
    reportResult(RESULT_SUCCESS, sStartTime + funcStr);
    return numChans;
}

int HatInterface::readCalCoeffs(uint16_t devType, uint8_t address, uint8_t chan, double &slope, double &offset)
{
    QString nameOfFunc, funcArgs, funcStr;
    QString argVals;
    QTime t;
    QString sStartTime;
    double chanSlope, chanOffset;

    funcArgs = "(address, chan, &slope, &offset)\n";
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
    argVals = QString("(%1, %2, %3, %4)")
            .arg(address)
            .arg(chan)
            .arg(chanSlope)
            .arg(chanOffset);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    slope = chanSlope;
    offset = chanOffset;
    return mResponse;
}

int HatInterface::writeCalCoeffs(uint16_t devType, uint8_t address, uint8_t chan, double slope, double offset)
{
    QString nameOfFunc, funcArgs, funcStr;
    QString argVals;
    QTime t;
    QString sStartTime;

    funcArgs = "(address, chan, slope, offset) = result\n";
    switch (devType) {
    case HAT_ID_MCC_118:
        nameOfFunc = "118: WriteCal";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_calibration_coefficient_write(address, chan, slope, offset);
        break;
    case 323:
        //to do: change to constant HAT_ID_MCC_134
        nameOfFunc = "134: numAIChans";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc134_calibration_coefficient_write(address, chan, slope, offset);
        break;
    default:
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QStringLiteral("(%1, %2, %3, %4)")
            .arg(address)
            .arg(chan)
            .arg(slope)
            .arg(offset);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals + QString(" [Error = %1]").arg(mResponse);
    reportResult(mResponse, sStartTime + funcStr);
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

int HatInterface::readTcTypes(uint16_t devType, uint8_t address, uint8_t chan, uint8_t &tcType)
{
    QString nameOfFunc, funcArgs, funcStr;
    QString argVals;
    QTime t;
    QString sStartTime;
    uint8_t typeOfTc;

    funcArgs = "(address, chan, &tcType)\n";
    switch (devType) {
    case 323:
        //to do: change to constant HAT_ID_MCC_134
        nameOfFunc = "134: readTcTypes";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc134_tc_type_read(address, chan, &typeOfTc);
        break;
    default:
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QString("(%1, %2, %3)")
            .arg(address)
            .arg(chan)
            .arg(typeOfTc);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    tcType = typeOfTc;
    return mResponse;
}

int HatInterface::writeTcType(uint16_t devType, uint8_t address, uint8_t chan, uint8_t tcType)
{
    QString nameOfFunc, funcArgs, funcStr;
    QString argVals;
    QTime t;
    QString sStartTime;

    funcArgs = "(address, chan, tcType)\n";
    switch (devType) {
    case 323:
        //to do: change to constant HAT_ID_MCC_134
        nameOfFunc = "134: writeTcType";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc134_tc_type_write(address, chan, tcType);
        break;
    default:
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QString("(%1, %2, %3)")
            .arg(address)
            .arg(chan)
            .arg(tcType);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    return mResponse;
}

int HatInterface::boardTemp(uint16_t devType, uint8_t address, double &temp)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    double data;
    uint8_t chan;

    chan = 0;
    funcArgs = "(mAddress, curChan, &data, NULL, NULL)\n";
    switch (devType) {
    case 323:
        //to do: change to constant HAT_ID_MCC_134
        nameOfFunc = "134: BoardTemp";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc134_t_in_read(address, chan, NULL, NULL, &data);
        //mResponse = RESULT_INVALID_DEVICE;
        break;
    default:
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QStringLiteral("(%1, %2, %3, %4, %5)")
            .arg(address)
            .arg(chan)
            .arg("NULL")
            .arg("NULL")
            .arg(data);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    temp = data;
    return mResponse;
}

int HatInterface::tInRead(uint16_t devType, uint8_t address, uint8_t chan, double &temp)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    double data;

    funcArgs = "(mAddress, curChan, &data, NULL, NULL)\n";
    switch (devType) {
    case 323:
        //to do: change to constant HAT_ID_MCC_134
        nameOfFunc = "134: TInRead";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc134_t_in_read(address, chan, &data, NULL, NULL);
        //mResponse = RESULT_INVALID_DEVICE;
        break;
    default:
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QStringLiteral("(%1, %2, %3, %4, %5)")
            .arg(address)
            .arg(chan)
            .arg(data)
            .arg("NULL")
            .arg("NULL");
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    temp = data;
    return mResponse;
}

int HatInterface::vInRead(uint16_t devType, uint8_t address, uint8_t chan, double &volts)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    double data;

    funcArgs = "(mAddress, curChan, &data, NULL, NULL)\n";
    switch (devType) {
    case 323:
        //to do: change to constant HAT_ID_MCC_134
        nameOfFunc = "134: VInRead";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc134_t_in_read(address, chan, NULL, &data, NULL);
        //mResponse = RESULT_INVALID_DEVICE;
        break;
    default:
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QStringLiteral("(%1, %2, %3, %4, %5)")
            .arg(address)
            .arg(chan)
            .arg("NULL")
            .arg(data)
            .arg("NULL");
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    volts = data;
    return mResponse;
}

int HatInterface::setTrigger(uint16_t devType, uint8_t address, TriggerMode trigType)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;

    funcArgs = "(mAddress, mTriggerType)\n";
    switch (devType) {
    case HAT_ID_MCC_118:
        nameOfFunc = "118: TrigMode";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_trigger_mode(address, trigType);
        break;
    default:
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QString("(%1, %2)")
            .arg(address)
            .arg(trigType);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    if (mResponse!=RESULT_SUCCESS) {
        mMainWindow->setError(mResponse, sStartTime + funcStr);
    } else {
        mMainWindow->addFunction(sStartTime + funcStr);
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

void HatInterface::addToMenu(uint16_t devType, uint8_t address, QString devName)
{
    mMainWindow->addDeviceToMenu(devName, address, devType);
}

void HatInterface::removeFromMenu(uint8_t address)
{
    mMainWindow->removeDeviceFromMenu(address);
}

void HatInterface::reportResult(int response, QString description)
{
    if (response != RESULT_SUCCESS) {
        mMainWindow->setError(response, description);
    } else {
        mMainWindow->addFunction(description);
    }
}
