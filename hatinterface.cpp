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

QString HatInterface::getErrorMessage(int result)
{
    const char* errText;
    QString returnText;

    errText = hat_error_message(result);
    returnText = QString("%1").arg(errText);
    return returnText;
}

int HatInterface::openDevice(uint16_t devType, uint8_t address)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": Open");
    funcArgs = "(mAddress)\n";
    switch (devType) {
    case HAT_ID_MCC_118:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_open(address);
        break;
#ifdef HAT_03
    case HAT_ID_MCC_134:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc134_open(address);
        break;
#endif
#ifdef HAT_04
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc152_open(address);
        break;
#endif
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
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
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": Close");
    funcArgs = "(mAddress)\n";
    switch (devType) {
    case HAT_ID_MCC_118:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_close(address);
        break;
#ifdef HAT_03
    case HAT_ID_MCC_134:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc134_close(address);
        break;
#endif
#ifdef HAT_04
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc152_close(address);
        break;
#endif
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
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
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": IsOpen");
    funcArgs = "(address) = result\n";
    switch (devType) {
    case HAT_ID_MCC_118:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        isOpen = mcc118_is_open(address);
        break;
#ifdef HAT_03
    case HAT_ID_MCC_134:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        isOpen = mcc134_is_open(address);
        break;
#endif
#ifdef HAT_04
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        isOpen = mcc152_is_open(address);
        break;
#endif
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
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

int HatInterface::getSerialNumber(uint16_t devType, uint8_t address, QString &serialNum)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    char serNum[10];
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": serialNum");
    serialNum = "";
    funcArgs = "(mAddress, serNum)\n";
    switch (devType) {
    case HAT_ID_MCC_118:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_serial(address, serNum);
        break;
#ifdef HAT_03
    case HAT_ID_MCC_134:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc134_serial(address, serNum);
        break;
#endif
#ifdef HAT_04
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc152_serial(address, serNum);
        break;
#endif
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }

    argVals = QString("(%1, %2)")
            .arg(address)
            .arg(serNum);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    if (mResponse == RESULT_SUCCESS) {
        serialNum = QString("%1").arg(serNum);
    }
    return mResponse;
}

int HatInterface::getFirmwareVersion(uint16_t devType, uint8_t address, uint16_t& fwMain, uint16_t& fwBoot)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    uint16_t version, boot;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": firmwareVersion");
    funcArgs = "(address, &version, &boot)\n";
    version = 0;
    boot = 0;
    switch (devType) {
    case HAT_ID_MCC_118:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_firmware_version(address, &version, &boot);
        break;
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
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
    if (mResponse == RESULT_SUCCESS) {
        fwMain = version;
        fwBoot = boot;
    }
    return mResponse;
}

int HatInterface::blinkLED(uint16_t devType, uint8_t address, uint8_t count)
{
    QString nameOfFunc, funcArgs, funcStr;
    QString argVals;
    QTime t;
    QString sStartTime;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": BlinkLED");
    funcArgs = "(address, flashCount)\n";
    switch (devType) {
    case HAT_ID_MCC_118:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_blink_led(address, count);
        break;
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QString("(%1, %2)")
            .arg(address)
            .arg(count);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    return mResponse;
}

int HatInterface::readCalDate(uint16_t devType, uint8_t address, QString &calDate)
{
    QString nameOfFunc, funcArgs, funcStr;
    QString argVals;
    QTime t;
    QString sStartTime;
    char dateReturned[12];
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": CalDate");
    calDate = "";
    funcArgs = "(address, calDate)\n";
    switch (devType) {
    case HAT_ID_MCC_118:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_calibration_date(address, dateReturned);
        break;
#ifdef HAT_03
    case HAT_ID_MCC_134:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc134_calibration_date(address, dateReturned);
        break;
#endif
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QString("(%1, %2)")
            .arg(address)
            .arg(dateReturned);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    if (mResponse == RESULT_SUCCESS) {
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
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": numAIChans");
    funcArgs = "() = result\n";
    switch (devType) {
    case HAT_ID_MCC_118:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        numChans = mcc118_a_in_num_channels();
        break;
#ifdef HAT_03
    case HAT_ID_MCC_134:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        numChans = mcc134_a_in_num_channels();
        break;
#endif
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
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
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": readCalCoeffs");
    funcArgs = "(address, chan, &slope, &offset)\n";
    switch (devType) {
    case HAT_ID_MCC_118:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_calibration_coefficient_read(address, chan, &chanSlope, &chanOffset);
        break;
#ifdef HAT_03
    case HAT_ID_MCC_134:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc134_calibration_coefficient_read(address, chan, &chanSlope, &chanOffset);
        break;
#endif
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
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
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": WriteCal");
    funcArgs = "(address, chan, slope, offset) = result\n";
    switch (devType) {
    case HAT_ID_MCC_118:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_calibration_coefficient_write(address, chan, slope, offset);
        break;
#ifdef HAT_03
    case HAT_ID_MCC_134:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc134_calibration_coefficient_write(address, chan, slope, offset);
        break;
#endif
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
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
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": AInRead");
    funcArgs = "(mAddress, curChan, mScanOptions, &data)\n";
    switch (devType) {
    case HAT_ID_MCC_118:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_a_in_read(address, chan, options, &data);
        break;
#ifdef HAT_03
    case HAT_ID_MCC_134:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc134_a_in_read(address, chan, options, &data);
        break;
#endif
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
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
    reportResult(mResponse, sStartTime + funcStr);
    value = data;
    return mResponse;
}

int HatInterface::testClock(uint16_t devType, uint8_t address, uint8_t mode, uint8_t &value)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    uint8_t valReturned;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": ClockTest");
    funcArgs = "(address, &mode, &value)\n";
    valReturned = 0;
    switch (devType) {
    case HAT_ID_MCC_118:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_test_clock(address, mode, &valReturned);
        break;
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QString("(%1, %2, %3)")
            .arg(address)
            .arg(mode)
            .arg(valReturned);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    value = valReturned;
    return mResponse;
}

int HatInterface::testTrig(uint16_t devType, uint8_t address, uint8_t &value)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    uint8_t valReturned;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": TrigTest");
    funcArgs = "(address, &value)\n";
    valReturned = 0;
    switch (devType) {
    case HAT_ID_MCC_118:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_test_trigger(address, &valReturned);
        break;
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QString("(%1, %2)")
            .arg(address)
            .arg(valReturned);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    value = valReturned;
    return mResponse;
}

int HatInterface::setTrigger(uint16_t devType, uint8_t address, TriggerMode trigType)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": TrigMode");
    funcArgs = "(mAddress, mTriggerType)\n";
    switch (devType) {
    case HAT_ID_MCC_118:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_trigger_mode(address, trigType);
        break;
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QString("(%1, %2)")
            .arg(address)
            .arg(trigType);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    return mResponse;
}

int HatInterface::readAInScanStatus(uint16_t devType, uint8_t address, uint16_t &status, uint32_t &sampsAvailable)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    uint16_t statReturned;
    uint32_t numRead;
    QString hatName;
    int response;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": AInScanStatus");
    funcArgs = "(mAddress, status, samplesAvailable)\n";
    switch (devType) {
    case HAT_ID_MCC_118:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_a_in_scan_status(address, &statReturned, &numRead);
        //mResponse = mcc118_a_in_scan_read(address, &statReturned, 0, 0, NULL, 0, &numRead);
        break;
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QString("(%1, %2, %3)")
            .arg(address)
            .arg(statReturned)
            .arg(numRead);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    response = mResponse;
    if(mResponse == RESULT_RESOURCE_UNAVAIL) {
        response = RESULT_SUCCESS;
    }
    reportResult(response, sStartTime + funcStr);
    sampsAvailable = numRead;
    status = statReturned;
    return mResponse;
}

int HatInterface::stopAInScan(uint16_t devType, uint8_t address)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": AInScanStop");
    funcArgs = "(mAddress)\n";
    switch (devType) {
    case HAT_ID_MCC_118:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_a_in_scan_stop(address);
        break;
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QString("(%1)").arg(address);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    return mResponse;
}

int HatInterface::aInScanChanCount(uint16_t devType, uint8_t address)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime, hatName;
    int chanCount;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": AInScanChanCount");
    chanCount = 0;
    mResponse = RESULT_SUCCESS;
    funcArgs = "(address)\n";
    switch (devType) {
    case HAT_ID_MCC_118:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        chanCount = mcc118_a_in_scan_channel_count(address);
        break;
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QStringLiteral("(%1)").arg(address);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    return chanCount;
}

int HatInterface::aInScanCleanup(uint16_t devType, uint8_t address)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": AInScanCleanup");
    funcArgs = "(address)\n";
    switch (devType) {
    case HAT_ID_MCC_118:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_a_in_scan_cleanup(address);
        break;
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QStringLiteral("(%1)").arg(address);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
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

void HatInterface::updateAppStatus(UtFunctionGroup funcGroup, QString statusText)
{
    QString groupName = getFuncGroupName(funcGroup);
    mMainWindow->setStatusLabel(groupName + ": " +  statusText);
}

#ifdef HAT_03

int HatInterface::boardTemp(uint16_t devType, uint8_t address, double &temp)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    double data;
    uint8_t chan;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": ReadCJC");
    chan = 0;
    funcArgs = "(mAddress, curChan, &data)\n";
    switch (devType) {
    case HAT_ID_MCC_134:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc134_cjc_read(address, chan, &data);
        break;
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QStringLiteral("(%1, %2, %3)")
            .arg(address)
            .arg(chan)
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
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": TInRead");
    funcArgs = "(mAddress, curChan, &data)\n";
    switch (devType) {
    case HAT_ID_MCC_134:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc134_t_in_read(address, chan, &data);
        break;
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QStringLiteral("(%1, %2, %3)")
            .arg(address)
            .arg(chan)
            .arg(data);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    temp = data;
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
    case HAT_ID_MCC_134:
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
    case HAT_ID_MCC_134:
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

#endif

#ifdef HAT_04

int HatInterface::getNumAOutChans(uint16_t devType)
{
    QString nameOfFunc, funcArgs, funcStr;
    QString argVals;
    QTime t;
    QString sStartTime;
    int numChans;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": numAOChans");
    funcArgs = "() = result\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        numChans = mcc152_a_out_num_channels();
        break;
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        numChans = 0;
        break;
    }
    argVals = "()";
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(numChans);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals + QString(" = %1").arg(numChans);
    reportResult(RESULT_SUCCESS, sStartTime + funcStr);
    return numChans;
}

int HatInterface::getNumDioChans(uint16_t devType)
{
    QString nameOfFunc, funcArgs, funcStr;
    QString argVals;
    QTime t;
    QString sStartTime;
    int numChans;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": numDioChans");
    funcArgs = "() = result\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        numChans = mcc152_dio_num_channels();
        break;
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        numChans = 0;
        break;
    }
    argVals = "()";
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(numChans);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals + QString(" = %1").arg(numChans);
    reportResult(RESULT_SUCCESS, sStartTime + funcStr);
    return numChans;
}

int HatInterface::aOutWrite(uint16_t devType, uint8_t address, uint8_t chan, uint32_t options, double value)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": AOutWrite");
    funcArgs = "(mAddress, curChan, mScanOptions, data)\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc152_a_out_write(address, chan, options, data);
        break;
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
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
    reportResult(mResponse, sStartTime + funcStr);
    return mResponse;
}

int HatInterface::aOutWriteAll(uint16_t devType, uint8_t address, uint32_t options, double &values)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": AOutWriteAll");
    funcArgs = "(mAddress, mScanOptions, data)\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc152_a_out_write_all(address, options, &values);
        break;
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QStringLiteral("(%1, %2, %3)")
            .arg(address)
            .arg(options)
            .arg(data);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    return mResponse;
}

int HatInterface::dioReset(uint16_t devType, uint8_t address)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": dioReset");
    funcArgs = "(mAddress)\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc152_dio_reset(address);
        break;
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QStringLiteral("(%1)")
            .arg(address);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    return mResponse;
}

int HatInterface::dioInputRead(uint16_t devType, uint8_t address, uint8_t chan, uint8_t &value)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    uint8_t data;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": dioInputRead");
    funcArgs = "(mAddress, curChan, &data)\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc152_dio_input_read(address, chan, &data);
        break;
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QStringLiteral("(%1, %2, %3)")
            .arg(address)
            .arg(chan)
            .arg(data);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    value = data;
    return mResponse;
}

int HatInterface::dioOutputWrite(uint16_t devType, uint8_t address, uint8_t chan, uint8_t value)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": dioOutputWrite");
    funcArgs = "(mAddress, curChan, value)\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc152_dio_output_write(address, chan, value);
        break;
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QStringLiteral("(%1, %2, %3)")
            .arg(address)
            .arg(chan)
            .arg(value);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    return mResponse;
}

int HatInterface::dioOutputRead(uint16_t devType, uint8_t address, uint8_t chan, uint8_t &value)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    uint8_t data;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": dioOutputRead");
    funcArgs = "(mAddress, curChan, &data)\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc152_dio_output_read(address, chan, &data);
        break;
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QStringLiteral("(%1, %2, %3)")
            .arg(address)
            .arg(chan)
            .arg(data);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    value = data;
    return mResponse;
}

int HatInterface::dioDirectionWrite(uint16_t devType, uint8_t address, uint8_t chan, uint8_t value)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": dioDirectionWrite");
    funcArgs = "(mAddress, curChan, value)\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc152_dio_direction_write(address, chan, value);
        break;
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QStringLiteral("(%1, %2, %3)")
            .arg(address)
            .arg(chan)
            .arg(value);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    return mResponse;
}

int HatInterface::dioDirectionRead(uint16_t devType, uint8_t address, uint8_t chan, uint8_t &value)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    uint8_t data;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": dioDirectionRead");
    funcArgs = "(mAddress, curChan, &data)\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc152_dio_direction_read(address, chan, &data);
        break;
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QStringLiteral("(%1, %2, %3)")
            .arg(address)
            .arg(chan)
            .arg(data);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    value = data;
    return mResponse;
}

int HatInterface::dioPullConfigWrite(uint16_t devType, uint8_t address, uint8_t chan, uint8_t value)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": dioPullConfigWrite");
    funcArgs = "(mAddress, curChan, value)\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc152_dio_pull_config_write(address, chan, value);
        break;
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QStringLiteral("(%1, %2, %3)")
            .arg(address)
            .arg(chan)
            .arg(value);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    return mResponse;
}

int HatInterface::dioPullConfigRead(uint16_t devType, uint8_t address, uint8_t chan, uint8_t &value)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    uint8_t data;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": dioPullConfigRead");
    funcArgs = "(mAddress, curChan, &data)\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc152_dio_pull_config_read(address, chan, &data);
        break;
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QStringLiteral("(%1, %2, %3)")
            .arg(address)
            .arg(chan)
            .arg(data);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    value = data;
    return mResponse;
}

int HatInterface::dioPullEnableWrite(uint16_t devType, uint8_t address, uint8_t chan, uint8_t value)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": dioPullEnableWrite");
    funcArgs = "(mAddress, curChan, value)\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc152_dio_pull_enable_write(address, chan, value);
        break;
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QStringLiteral("(%1, %2, %3)")
            .arg(address)
            .arg(chan)
            .arg(value);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    return mResponse;
}

int HatInterface::dioPullEnableRead(uint16_t devType, uint8_t address, uint8_t chan, uint8_t &value)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    uint8_t data;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": dioPullEnableRead");
    funcArgs = "(mAddress, curChan, &data)\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc152_dio_pull_enable_read(address, chan, &data);
        break;
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QStringLiteral("(%1, %2, %3)")
            .arg(address)
            .arg(chan)
            .arg(data);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    value = data;
    return mResponse;
}

int HatInterface::dioInputInvertWrite(uint16_t devType, uint8_t address, uint8_t chan, uint8_t value)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": dioInputInvertWrite");
    funcArgs = "(mAddress, curChan, value)\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc152_dio_input_invert_write(address, chan, value);
        break;
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QStringLiteral("(%1, %2, %3)")
            .arg(address)
            .arg(chan)
            .arg(value);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    return mResponse;
}

int HatInterface::dioInputInvertRead(uint16_t devType, uint8_t address, uint8_t chan, uint8_t &value)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    uint8_t data;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": dioInputInvertRead");
    funcArgs = "(mAddress, curChan, &data)\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc152_dio_input_invert_read(address, chan, &data);
        break;
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QStringLiteral("(%1, %2, %3)")
            .arg(address)
            .arg(chan)
            .arg(data);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    value = data;
    return mResponse;
}

int HatInterface::dioInputLatchWrite(uint16_t devType, uint8_t address, uint8_t chan, uint8_t value)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": dioInputLatchWrite");
    funcArgs = "(mAddress, curChan, value)\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc152_dio_input_latch_write(address, chan, value);
        break;
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QStringLiteral("(%1, %2, %3)")
            .arg(address)
            .arg(chan)
            .arg(value);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    return mResponse;
}

int HatInterface::dioInputLatchRead(uint16_t devType, uint8_t address, uint8_t chan, uint8_t &value)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    uint8_t data;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": dioInputLatchRead");
    funcArgs = "(mAddress, curChan, &data)\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc152_dio_input_latch_read(address, chan, &data);
        break;
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QStringLiteral("(%1, %2, %3)")
            .arg(address)
            .arg(chan)
            .arg(data);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    value = data;
    return mResponse;
}

int HatInterface::dioOutputTypeWrite(uint16_t devType, uint8_t address, uint8_t value)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": dioOutputTypeWrite");
    funcArgs = "(mAddress, curChan, value)\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc152_dio_output_type_write(address, value);
        break;
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QStringLiteral("(%1, %2)")
            .arg(address)
            .arg(value);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    return mResponse;
}

int HatInterface::dioOutputTypeRead(uint16_t devType, uint8_t address, uint8_t &value)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    uint8_t data;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": dioOutputTypeRead");
    funcArgs = "(mAddress, curChan, &data)\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc152_dio_output_type_read(address, chan, &data);
        break;
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QStringLiteral("(%1, %2, %3)")
            .arg(address)
            .arg(chan)
            .arg(data);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    value = data;
    return mResponse;
}

int HatInterface::dioInterruptMaskWrite(uint16_t devType, uint8_t address, uint8_t chan, uint8_t value)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": dioInterruptMaskWrite");
    funcArgs = "(mAddress, curChan, value)\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc152_dio_interrupt_mask_write(address, chan, value);
        break;
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QStringLiteral("(%1, %2, %3)")
            .arg(address)
            .arg(chan)
            .arg(value);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    return mResponse;
}

int HatInterface::dioInterruptMaskRead(uint16_t devType, uint8_t address, uint8_t chan, uint8_t &value)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    uint8_t data;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": dioInterruptMaskRead");
    funcArgs = "(mAddress, curChan, &data)\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc152_dio_interrupt_mask_read(address, chan, &data);
        break;
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QStringLiteral("(%1, %2, %3)")
            .arg(address)
            .arg(chan)
            .arg(data);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    value = data;
    return mResponse;
}

int HatInterface::dioInterruptStatusRead(uint16_t devType, uint8_t address, uint8_t chan, uint8_t &value)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    uint8_t data;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": dioInterruptStatusRead");
    funcArgs = "(mAddress, curChan, &data)\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc152_dio_interrupt_status_read(address, chan, &data);
        break;
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QStringLiteral("(%1, %2, %3)")
            .arg(address)
            .arg(chan)
            .arg(data);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    value = data;
    return mResponse;
}

#endif
