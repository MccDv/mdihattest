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
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;

    nameOfFunc = "ErrorMessage";
    funcArgs = "(errorCode)\n";
    sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
    errText = hat_error_message(result);
    returnText = QString("%1").arg(errText);

    argVals = QString("(%1)").arg(result);
    mStatusString = nameOfFunc + argVals;

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(RESULT_SUCCESS, sStartTime + funcStr);
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
#ifdef HAT_05
    case HAT_ID_MCC_172:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc172_open(address);
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
#ifdef HAT_05
    case HAT_ID_MCC_172:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc172_close(address);
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
#ifdef HAT_05
    case HAT_ID_MCC_172:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        isOpen = mcc172_is_open(address);
        break;
#endif
    default:
        funcArgs = "(~) = result\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        isOpen = 0;
        argVals = "(~)";
        break;
    }

    if(argVals == "")
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
#ifdef HAT_05
    case HAT_ID_MCC_172:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc172_serial(address, serNum);
        break;
#endif
    default:
        funcArgs = "(~)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        argVals = "(~)";
        break;
    }

    if(argVals == "")
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
#ifdef HAT_05
    case HAT_ID_MCC_172:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc172_firmware_version(address, &version);
        break;
#endif
    default:
        funcArgs = "(~)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        argVals = "(~)";
        break;
    }

    if(argVals == "")
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
#ifdef HAT_05
    case HAT_ID_MCC_172:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc172_blink_led(address, count);
        break;
#endif
    default:
        funcArgs = "(~)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        argVals = "(~)";
        break;
    }

    if(argVals == "")
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
#ifdef HAT_05
    case HAT_ID_MCC_172:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc172_calibration_date(address, dateReturned);
        break;
#endif
    default:
        funcArgs = "(~)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        argVals = "(~)";
        break;
    }

    if(argVals == "")
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
        numChans = mcc118_info()->NUM_AI_CHANNELS;
        break;
#ifdef HAT_03
    case HAT_ID_MCC_134:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        numChans = mcc134_info()->NUM_AI_CHANNELS;
        break;
#endif
#ifdef HAT_05
    case HAT_ID_MCC_172:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        numChans = mcc172_info()->NUM_AI_CHANNELS;
        break;
#endif
    default:
        funcArgs = "(~) = 0\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        numChans = 0;
        break;
    }
    argVals = "()";
    mStatusString = nameOfFunc + argVals + QString(" = %1").arg(numChans);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals + QString(" = %1").arg(numChans);
    reportResult(RESULT_SUCCESS, sStartTime + funcStr);
    return numChans;
}

int32_t HatInterface::getAInCodeMax(uint16_t devType)
{
    QString nameOfFunc, funcArgs, funcStr;
    QString argVals;
    QTime t;
    QString sStartTime;
    int32_t maxCode;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": aInMaxCode");
    funcArgs = "() = maxCode\n";
    switch (devType) {
    case HAT_ID_MCC_118:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        maxCode = mcc118_info()->AI_MAX_CODE;
        break;
#ifdef HAT_05
    case HAT_ID_MCC_172:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        maxCode = mcc172_info()->AI_MAX_CODE;
        break;
#endif
    default:
        funcArgs = "(~) = 0\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        maxCode = 0;
        break;
    }
    argVals = "()";
    mStatusString = nameOfFunc + argVals + QString(" = %1").arg(maxCode);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals + QString(" = %1").arg(maxCode);
    reportResult(RESULT_SUCCESS, sStartTime + funcStr);
    return maxCode;
}

int32_t HatInterface::getAInCodeMin(uint16_t devType)
{
    QString nameOfFunc, funcArgs, funcStr;
    QString argVals;
    QTime t;
    QString sStartTime;
    int32_t minCode;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": aInMinCode");
    funcArgs = "() = minCode\n";
    switch (devType) {
    case HAT_ID_MCC_118:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        minCode = mcc118_info()->AI_MIN_CODE;
        break;
#ifdef HAT_05
    case HAT_ID_MCC_172:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        minCode = mcc172_info()->AI_MIN_CODE;
        break;
#endif
    default:
        funcArgs = "(~) = 0\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        minCode = 0;
        break;
    }
    argVals = "()";
    mStatusString = nameOfFunc + argVals + QString(" = %1").arg(minCode);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals + QString(" = %1").arg(minCode);
    reportResult(RESULT_SUCCESS, sStartTime + funcStr);
    return minCode;
}

double HatInterface::getAInRangeMax(uint16_t devType)
{
    QString nameOfFunc, funcArgs, funcStr;
    QString argVals;
    QTime t;
    QString sStartTime;
    double maxRange;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": aInMaxRange");
    funcArgs = "() = maxRange\n";
    switch (devType) {
    case HAT_ID_MCC_118:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        maxRange = mcc118_info()->AI_MAX_RANGE;
        break;
#ifdef HAT_03
    case HAT_ID_MCC_134:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        maxRange = (mcc134_info()->AI_MAX_RANGE) * 1000;
        break;
#endif
#ifdef HAT_05
    case HAT_ID_MCC_172:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        maxRange = mcc172_info()->AI_MAX_RANGE;
        break;
#endif
    default:
        funcArgs = "(~) = 0\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        maxRange = 0;
        break;
    }
    argVals = "()";
    mStatusString = nameOfFunc + argVals + QString(" = %1").arg(maxRange);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals + QString(" = %1").arg(maxRange);
    reportResult(RESULT_SUCCESS, sStartTime + funcStr);
    return maxRange;
}

double HatInterface::getAInRangeMin(uint16_t devType)
{
    QString nameOfFunc, funcArgs, funcStr;
    QString argVals;
    QTime t;
    QString sStartTime;
    double minRange;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": aInMinRange");
    funcArgs = "() = minRange\n";
    switch (devType) {
    case HAT_ID_MCC_118:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        minRange = mcc118_info()->AI_MIN_RANGE;
        break;
#ifdef HAT_03
    case HAT_ID_MCC_134:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        minRange = (mcc134_info()->AI_MIN_RANGE) * 1000;
        break;
#endif
#ifdef HAT_05
    case HAT_ID_MCC_172:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        minRange = mcc172_info()->AI_MIN_RANGE;
        break;
#endif
    default:
        funcArgs = "(~) = 0\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        minRange = 0;
        break;
    }
    argVals = "()";
    mStatusString = nameOfFunc + argVals + QString(" = %1").arg(minRange);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals + QString(" = %1").arg(minRange);
    reportResult(RESULT_SUCCESS, sStartTime + funcStr);
    return minRange;
}

double HatInterface::getAInVoltsMax(uint16_t devType)
{
    QString nameOfFunc, funcArgs, funcStr;
    QString argVals;
    QTime t;
    QString sStartTime;
    QString hatName;
    int prec;
    double maxVolts;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": aInMaxVolts");
    funcArgs = "() = maxVolts\n";
    prec = 6;
    switch (devType) {
    case HAT_ID_MCC_118:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        maxVolts = mcc118_info()->AI_MAX_VOLTAGE;
        break;
#ifdef HAT_03
    case HAT_ID_MCC_134:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        maxVolts = (mcc134_info()->AI_MAX_VOLTAGE) * 1000;
        break;
#endif
#ifdef HAT_05
    case HAT_ID_MCC_172:
        prec = 12;
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        maxVolts = mcc172_info()->AI_MAX_VOLTAGE;
        break;
#endif
    default:
        funcArgs = "(~) = 0\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        maxVolts = 0;
        break;
    }
    argVals = "()";
    mStatusString = nameOfFunc + argVals + QString(" = %1").arg(maxVolts);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals + QString(" = %1")
            .arg(maxVolts, 0, 'e', prec);
    reportResult(RESULT_SUCCESS, sStartTime + funcStr);
    return maxVolts;
}

double HatInterface::getAInVoltsMin(uint16_t devType)
{
    QString nameOfFunc, funcArgs, funcStr;
    QString argVals;
    QTime t;
    QString sStartTime;
    QString hatName;
    int prec;
    double minVolts;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": aInMinVolts");
    funcArgs = "() = minVolts\n";
    prec = 6;
    switch (devType) {
    case HAT_ID_MCC_118:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        minVolts = mcc118_info()->AI_MIN_VOLTAGE;
        break;
#ifdef HAT_03
    case HAT_ID_MCC_134:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        minVolts = (mcc134_info()->AI_MIN_VOLTAGE) * 1000;
        break;
#endif
#ifdef HAT_05
    case HAT_ID_MCC_172:
        prec = 12;
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        minVolts = mcc172_info()->AI_MIN_VOLTAGE;
        break;
#endif
    default:
        funcArgs = "(~) = 0\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        minVolts = 0;
        break;
    }
    argVals = "()";
    mStatusString = nameOfFunc + argVals + QString(" = %1")
            .arg(minVolts, 0, 'e', prec);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals + QString(" = %1").arg(minVolts);
    reportResult(RESULT_SUCCESS, sStartTime + funcStr);
    return minVolts;
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
#ifdef HAT_05
    case HAT_ID_MCC_172:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc172_calibration_coefficient_read(address, chan, &chanSlope, &chanOffset);
        break;
#endif
    default:
        funcArgs = "(~)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        argVals = "(~)";
        break;
    }

    if(argVals == "")
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
    funcArgs = "(address, chan, slope, offset)\n";
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
#ifdef HAT_05
    case HAT_ID_MCC_172:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc172_calibration_coefficient_write(address, chan, slope, offset);
        break;
#endif
    default:
        funcArgs = "(~)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }

    if(argVals == "")
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
        funcArgs = "(~)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        argVals = "(~)";
        break;
    }

    if(argVals == "")
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
        funcArgs = "(~)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        argVals = "(~)";
        break;
    }

    if(argVals == "")
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
        funcArgs = "(~)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        argVals = "(~)";
        break;
    }
    if(argVals == "")
        argVals = QString("(%1, %2)")
                .arg(address)
                .arg(valReturned);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    value = valReturned;
    return mResponse;
}

int HatInterface::setTrigger(uint16_t devType, uint8_t address, uint8_t source, TriggerMode trigType)
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
        (void)source;
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_trigger_mode(address, trigType);
        break;
#ifdef HAT_05
    case HAT_ID_MCC_172:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc172_trigger_config(address, source, trigType);
        break;
#endif
    default:
        funcArgs = "(~)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        argVals = "(~)";
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
        break;
#ifdef HAT_05
    case HAT_ID_MCC_172:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc172_a_in_scan_status(address, &statReturned, &numRead);
        break;
#endif
    default:
        funcArgs = "(~)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        argVals = "(~)";
        break;
    }

    if(argVals == "")
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
    argVals = QString("(%1)").arg(address);
    switch (devType) {
    case HAT_ID_MCC_118:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_a_in_scan_stop(address);
        break;
#ifdef HAT_05
    case HAT_ID_MCC_172:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc172_a_in_scan_stop(address);
        break;
#endif
    default:
        funcArgs = "(~)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        argVals = "(~)";
        break;
    }
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
    funcArgs = "(address) = chanCount\n";
    switch (devType) {
    case HAT_ID_MCC_118:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        chanCount = mcc118_a_in_scan_channel_count(address);
        break;
#ifdef HAT_05
    case HAT_ID_MCC_172:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        chanCount = mcc172_a_in_scan_channel_count(address);
        break;
#endif
    default:
        funcArgs = "(~)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        argVals = "(~)";
        break;
    }

    argVals = QStringLiteral("(%1) = %2").arg(address).arg(chanCount);
    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    return chanCount;
}

int HatInterface::getBufferSize(uint16_t devType, uint8_t address, uint32_t &bufferSize)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    uint32_t bufSize = 0;
    QString hatName;
    int response;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": AInScanBufSize");
    funcArgs = "(mAddress, bufferSize)\n";
    switch (devType) {
    case HAT_ID_MCC_118:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_a_in_scan_buffer_size(address, &bufSize);
        break;
#ifdef HAT_05
    case HAT_ID_MCC_172:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc172_a_in_scan_buffer_size(address, &bufSize);
        break;
#endif
    default:
        funcArgs = "(~)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QString("(%1, %2)")
            .arg(address)
            .arg(bufSize);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    response = mResponse;
    if(mResponse == RESULT_RESOURCE_UNAVAIL) {
        response = RESULT_SUCCESS;
    }
    reportResult(response, sStartTime + funcStr);
    bufferSize = bufSize;
    return mResponse;
}

int HatInterface::getAInScanParameters(uint16_t devType, uint8_t address, uint8_t chanCount, uint8_t &source, double &rate, uint8_t &value)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    uint8_t sync = 0;
    uint8_t sourceReturned;
    double rateReturned = 0.0;
    QString hatName;

    hatName = getHatTypeName(devType);
    sourceReturned = 0;
    sync = 0;
    switch (devType) {
    case HAT_ID_MCC_118:
        (void)address;
        nameOfFunc = hatName.append(": ainScanRate");
        funcArgs = "(chanCount, rate, &rateReturned)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_a_in_scan_actual_rate(chanCount, rate, &rateReturned);
        argVals = QStringLiteral("(%1, %2, %3)")
                .arg(chanCount)
                .arg(rate)
                .arg(rateReturned);
        break;
#ifdef HAT_05
    case HAT_ID_MCC_172:
        nameOfFunc = hatName.append(": ainClockConfigRead");
        funcArgs = "(mAddress, source, rate, &sync)\n";
        sourceReturned = SOURCE_LOCAL;
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc172_a_in_clock_config_read(address, &sourceReturned, &rateReturned, &sync);
        argVals = QStringLiteral("(%1, %2, %3, %4)")
                .arg(address)
                .arg(sourceReturned)
                .arg(rateReturned)
                .arg(sync);
        break;
#endif
    default:
        nameOfFunc = hatName.append(": getAinScanParameters");
        funcArgs = "(~)\n";
        argVals = "(~)";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    source = sourceReturned;
    rate = rateReturned;
    value = sync;
    return mResponse;
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
    argVals = QStringLiteral("(%1)").arg(address);
    switch (devType) {
    case HAT_ID_MCC_118:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_a_in_scan_cleanup(address);
        break;
#ifdef HAT_05
    case HAT_ID_MCC_172:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc172_a_in_scan_cleanup(address);
        break;
#endif
    default:
        funcArgs = "(~)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        argVals = "(~)";
        break;
    }
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

int HatInterface::boardTemp(uint16_t devType, uint8_t address, uint8_t chan, double &temp)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    double data;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": ReadCJC");
    funcArgs = "(mAddress, curChan, &data)\n";
    switch (devType) {
    case HAT_ID_MCC_134:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc134_cjc_read(address, chan, &data);
        break;
    default:
        funcArgs = "(~)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        argVals = "(~)";
        break;
    }

    if(argVals == "")
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
        funcArgs = "(~)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        argVals = "(~)";
        break;
    }
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    if(argVals == "")
        argVals = QStringLiteral("(%1, %2, %3)")
                .arg(address)
                .arg(chan)
                .arg(data);
    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    temp = data;
    return mResponse;
}

int HatInterface::readInterval(uint16_t devType, uint8_t address, uint8_t &interval)
{
    QString nameOfFunc, funcArgs, funcStr;
    QString argVals;
    QTime t;
    QString sStartTime, hatName;
    uint8_t intervalRead;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": readInterval");
    funcArgs = "(address, &interval)\n";
    switch (devType) {
    case HAT_ID_MCC_134:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc134_update_interval_read(address, &intervalRead);
        break;
    default:
        funcArgs = "(~)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        argVals = "(~)";
        break;
    }
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    if(argVals == "")
        argVals = QString("(%1, %2)")
                .arg(address)
                .arg(intervalRead);
    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    interval = intervalRead;
    return mResponse;
}

int HatInterface::writeInterval(uint16_t devType, uint8_t address, uint8_t interval)
{
    QString nameOfFunc, funcArgs, funcStr;
    QString argVals;
    QTime t;
    QString sStartTime, hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": writeInterval");
    funcArgs = "(address, interval)\n";
    switch (devType) {
    case HAT_ID_MCC_134:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc134_update_interval_write(address, interval);
        break;
    default:
        funcArgs = "(~)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        argVals = "(~)";
        break;
    }
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    if(argVals == "")
        argVals = QString("(%1, %2)")
                .arg(address)
                .arg(interval);
    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    return mResponse;
}

int HatInterface::readTcTypes(uint16_t devType, uint8_t address, uint8_t chan, uint8_t &tcType)
{
    QString nameOfFunc, funcArgs, funcStr;
    QString argVals;
    QTime t;
    QString sStartTime, hatName;
    uint8_t typeOfTc;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": readTcTypes");
    switch (devType) {
    case HAT_ID_MCC_134:
        funcArgs = "(address, chan, &tcType)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc134_tc_type_read(address, chan, &typeOfTc);
        break;
    default:
        funcArgs = "(~)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        argVals = "(~)";
        break;
    }
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    if(argVals == "")
        argVals = QString("(%1, %2, %3)")
                .arg(address)
                .arg(chan)
                .arg(typeOfTc);
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
    QString sStartTime, hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": writeTcType");
    funcArgs = "(address, chan, tcType)\n";
    switch (devType) {
    case HAT_ID_MCC_134:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc134_tc_type_write(address, chan, tcType);
        break;
    default:
        funcArgs = "(~)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        argVals = "(~)";
        break;
    }
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    if(argVals == "")
        argVals = QString("(%1, %2, %3)")
                .arg(address)
                .arg(chan)
                .arg(tcType);
    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    return mResponse;
}

#else

int HatInterface::boardTemp(uint16_t devType, uint8_t address, uint8_t chan, double &temp)
{
    (void)devType;
    (void)address;
    (void)chan;
    (void)temp;
    return RESULT_INVALID_DEVICE;
}

int HatInterface::tInRead(uint16_t devType, uint8_t address, uint8_t chan, double &temp)
{
    (void)devType;
    (void)address;
    (void)chan;
    (void)temp;
    return RESULT_INVALID_DEVICE;
}

int HatInterface::readInterval(uint16_t devType, uint8_t address, uint8_t &interval)
{
    (void)devType;
    (void)address;
    (void)interval;
    return RESULT_INVALID_DEVICE;
}

int HatInterface::writeInterval(uint16_t devType, uint8_t address, uint8_t interval)
{
    (void)devType;
    (void)address;
    (void)interval;
    return RESULT_INVALID_DEVICE;
}

int HatInterface::readTcTypes(uint16_t devType, uint8_t address, uint8_t chan, uint8_t &tcType)
{
    (void)devType;
    (void)address;
    (void)chan;
    (void)tcType;
    return RESULT_INVALID_DEVICE;
}

int HatInterface::writeTcType(uint16_t devType, uint8_t address, uint8_t chan, uint8_t tcType)
{
    (void)devType;
    (void)address;
    (void)chan;
    (void)tcType;
    return RESULT_INVALID_DEVICE;
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
    funcArgs = "() = AoChans\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        numChans = mcc152_info()->NUM_AO_CHANNELS;
        break;
    default:
        funcArgs = "(~) = AoChans\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        numChans = 0;
        argVals = "(~) = ~0";
        break;
    }

    if(argVals == "")
        argVals = QString("() = %1").arg(numChans);
    mStatusString = nameOfFunc + argVals;

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
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
    funcArgs = "() = dioChans\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        numChans = mcc152_info()->NUM_DIO_CHANNELS;
        break;
    default:
        funcArgs = "(~) = dioChans\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        numChans = 0;
        argVals = "(~) = ~0";
        break;
    }

    if(argVals == "")
        argVals = QString("() = %1").arg(numChans);
    mStatusString = nameOfFunc + argVals;

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(RESULT_SUCCESS, sStartTime + funcStr);
    return numChans;
}

int HatInterface::getAOutCodeMax(uint16_t devType)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    QString hatName;
    int codeMax;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": AOutCodeMax");
    funcArgs = "() = AoCode\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        codeMax = mcc152_info()->AO_MAX_CODE;
        mResponse = RESULT_SUCCESS;
        break;
    default:
        funcArgs = "(~) = AoCode\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        codeMax = 0;
        mResponse = RESULT_SUCCESS;
        argVals = "(~) = ~0";
        break;
    }
    if(argVals == "")
        argVals = QString("() = %1").arg(codeMax);
    mStatusString = nameOfFunc + argVals;

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    return codeMax;
}

int HatInterface::getAOutCodeMin(uint16_t devType)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    QString hatName;
    int codeMin;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": AOutCodeMin");
    funcArgs = "() = AoCode\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        codeMin = mcc152_info()->AO_MIN_CODE;
        mResponse = RESULT_SUCCESS;
        break;
    default:
        funcArgs = "(~) = AoCode\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        codeMin = 0;
        mResponse = RESULT_SUCCESS;
        argVals = "(~) = ~0";
        break;
    }

    if(argVals == "")
        argVals = QString("() = %1").arg(codeMin);
    mStatusString = nameOfFunc + argVals;

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals + QString(" = %1").arg(codeMin);
    reportResult(mResponse, sStartTime + funcStr);
    return codeMin;
}

double HatInterface::getAOutVoltsMin(uint16_t devType)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    QString hatName;
    double voltsMin;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": AOutVoltsMin");
    funcArgs = "() = AoVolts\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        voltsMin = mcc152_info()->AO_MIN_VOLTAGE;
        mResponse = RESULT_SUCCESS;
        break;
    default:
        funcArgs = "(~) = AoVolts\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        voltsMin = 0;
        mResponse = RESULT_SUCCESS;
        argVals = "(~) = ~0";
        break;
    }
    if(argVals == "")
        argVals = QString("() = %1").arg(voltsMin);
    mStatusString = nameOfFunc + argVals;

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    return voltsMin;
}

double HatInterface::getAOutVoltsMax(uint16_t devType)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    QString hatName;
    double voltsMax;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": AOutVoltsMax");
    funcArgs = "() = AoVolts\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        voltsMax = mcc152_info()->AO_MAX_VOLTAGE;
        mResponse = RESULT_SUCCESS;
        break;
    default:
        funcArgs = "(~) = AoVolts\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        voltsMax = 0;
        mResponse = RESULT_SUCCESS;
        argVals = "(~) = ~0";
        break;
    }

    if(argVals == "")
        argVals = QString("() = %1").arg(voltsMax);
    mStatusString = nameOfFunc + argVals;

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    return voltsMax;
}

double HatInterface::getAOutRangeMax(uint16_t devType)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    QString hatName;
    double voltsMax;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": AOutRangeMax");
    funcArgs = "() = AoRange\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        voltsMax = mcc152_info()->AO_MAX_RANGE;
        mResponse = RESULT_SUCCESS;
        break;
    default:
        funcArgs = "(~) = AoRange\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        voltsMax = 0;
        mResponse = RESULT_SUCCESS;
        argVals = "(~) = ~0";
        break;
    }

    if(argVals == "")
        argVals = QString("() = %1").arg(voltsMax);
    mStatusString = nameOfFunc + argVals;

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals + QString(" = %1").arg(voltsMax);
    reportResult(mResponse, sStartTime + funcStr);
    return voltsMax;
}

double HatInterface::getAOutRangeMin(uint16_t devType)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    QString hatName;
    double voltsMin;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": AOutRangeMin");
    funcArgs = "() = AoRange\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        voltsMin = mcc152_info()->AO_MIN_RANGE;
        mResponse = RESULT_SUCCESS;
        break;
    default:
        funcArgs = "(~) = AoRange\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        voltsMin = 0;
        mResponse = RESULT_SUCCESS;
        argVals = "(~) = ~0";
        break;
    }

    if(argVals == "")
        argVals = QString("() = %1").arg(voltsMin);
    mStatusString = nameOfFunc + argVals;

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals + QString(" = %1").arg(voltsMin);
    reportResult(mResponse, sStartTime + funcStr);
    return voltsMin;
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
        mResponse = mcc152_a_out_write(address, chan, options, value);
        break;
    default:
        funcArgs = "(~)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        argVals = "(~)";
        break;
    }

    if(argVals == "")
        argVals = QStringLiteral("(%1, %2, %3, %4)")
                .arg(address)
                .arg(chan)
                .arg(options)
                .arg(value);
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
        funcArgs = "(~)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        argVals = "(~)";
        break;
    }

    if(argVals == "")
        argVals = QStringLiteral("(%1, %2, %3)")
                .arg(address)
                .arg(options)
                .arg("values");
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
        funcArgs = "(~)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        argVals = "(~)";
        break;
    }

    if(argVals == "")
        argVals = QStringLiteral("(%1)")
                .arg(address);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    return mResponse;
}

int HatInterface::dioInBitRead(uint16_t devType, uint8_t address, uint8_t chan, uint8_t &value)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    uint8_t data;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": dioInBitRead");
    funcArgs = "(mAddress, curChan, &data)\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc152_dio_input_read_bit(address, chan, &data);
        break;
    default:
        funcArgs = "(~)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        argVals = "(~)";
        break;
    }

    if(argVals == "")
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

int HatInterface::dioInPortRead(uint16_t devType, uint8_t address, uint8_t &value)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    uint8_t data;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": dioInputReadAll");
    funcArgs = "(mAddress, &data)\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        data = 0;
        mResponse = mcc152_dio_input_read_port(address, &data);
        break;
    default:
        funcArgs = "(~)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        data = 0;
        mResponse = RESULT_INVALID_DEVICE;
        argVals = "(~)";
        break;
    }

    if(argVals == "")
        argVals = QStringLiteral("(%1, %2)")
                .arg(address)
                .arg(data);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    value = data;
    return mResponse;
}

int HatInterface::dioOutBitWrite(uint16_t devType, uint8_t address, uint8_t chan, uint8_t value)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": dioOutBitWrite");
    funcArgs = "(mAddress, curChan, value)\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc152_dio_output_write_bit(address, chan, value);
        break;
    default:
        funcArgs = "(~)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        argVals = "(~)";
        break;
    }

    if(argVals == "")
        argVals = QStringLiteral("(%1, %2, %3)")
                .arg(address)
                .arg(chan)
                .arg(value);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    return mResponse;
}

int HatInterface::dioOutPortWrite(uint16_t devType, uint8_t address, uint8_t value)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": dioOutputWriteAll");
    funcArgs = "(mAddress, value)\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc152_dio_output_write_port(address, value);
        break;
    default:
        funcArgs = "(mAddress, value)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        argVals = "(~)";
        break;
    }

    if(argVals == "")
        argVals = QStringLiteral("(%1, %2)")
                .arg(address)
                .arg(value);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    return mResponse;
}

int HatInterface::dioOutBitRead(uint16_t devType, uint8_t address, uint8_t chan, uint8_t &value)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    uint8_t data;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": dioOutBitRead");
    funcArgs = "(mAddress, curChan, &data)\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc152_dio_output_read_bit(address, chan, &data);
        break;
    default:
        funcArgs = "(~)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        argVals = "(~)";
        break;
    }

    if(argVals == "")
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

int HatInterface::dioOutPortRead(uint16_t devType, uint8_t address, uint8_t &value)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    uint8_t data;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": dioOutputReadAll");
    funcArgs = "(mAddress, &data)\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        data = 0;
        mResponse = mcc152_dio_output_read_port(address, &data);
        break;
    default:
        funcArgs = "(~)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        data = 0;
        mResponse = RESULT_INVALID_DEVICE;
        argVals = "(~)";
        break;
    }

    if(argVals == "")
        argVals = QStringLiteral("(%1, %2)")
                .arg(address)
                .arg(data);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    value = data;
    return mResponse;
}

int HatInterface::dioBitConfigWrite(uint16_t devType, uint8_t address, uint8_t chan, uint8_t item, uint8_t value)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": dioConfigWriteBit");
    funcArgs = "(mAddress, curChan, item, value)\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc152_dio_config_write_bit(address, chan, item, value);
        break;
    default:
        funcArgs = "(~)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        argVals = "(~)";
        break;
    }

    if(argVals == "")
        argVals = QStringLiteral("(%1, %2, %3, %4)")
                .arg(address)
                .arg(chan)
                .arg(getConfigItemName(item))
                .arg(value);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    return mResponse;
}

int HatInterface::dioBitConfigRead(uint16_t devType, uint8_t address, uint8_t chan, uint8_t item, uint8_t &value)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    QString hatName;
    uint8_t data;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": dioBitCfgRead");
    funcArgs = "(mAddress, curChan, item, &data)\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc152_dio_config_read_bit(address, chan, item, &data);
        break;
    default:
        funcArgs = "(~)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        argVals = "(~)";
        break;
    }

    if(argVals == "")
        argVals = QStringLiteral("(%1, %2, %3, %4)")
                .arg(address)
                .arg(chan)
                .arg(item)
                .arg(data);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    value = data;
    return mResponse;
}

int HatInterface::dioPortConfigWrite(uint16_t devType, uint8_t address, uint8_t item, uint8_t value)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": dioConfigWritePort");
    funcArgs = "(mAddress, item, value)\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc152_dio_config_write_port(address, item, value);
        break;
    default:
        funcArgs = "(~)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        argVals = "(~)";
        break;
    }

    if(argVals == "")
        argVals = QStringLiteral("(%1, %2, %3)")
                    .arg(address)
                .arg(getConfigItemName(item))
                .arg(value);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    return mResponse;
}

int HatInterface::dioPortConfigRead(uint16_t devType, uint8_t address, uint8_t item, uint8_t &value)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    uint8_t data;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": dioConfigReadPort");
    funcArgs = "(mAddress, item, &data)\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc152_dio_config_read_port(address, item, &data);
        break;
    default:
        funcArgs = "(~)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        argVals = "(~)";
        break;
    }

    if(argVals == "")
        argVals = QStringLiteral("(%1, %2, %3)")
                .arg(address)
                .arg(getConfigItemName(item))
                .arg(data);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    value = data;
    return mResponse;
}

int HatInterface::dioIntStatusBit(uint16_t devType, uint8_t address, uint8_t chan, uint8_t &value)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    QString hatName;
    uint8_t data;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": dioIntStatusBit");
    funcArgs = "(mAddress, curChan, &data)\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc152_dio_int_status_read_bit(address, chan, &data);
        break;
    default:
        funcArgs = "(~)\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        argVals = "(~)";
        break;
    }

    if(argVals == "")
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

int HatInterface::dioIntStatusPort(uint16_t devType, uint8_t address, uint8_t &value)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    uint8_t data;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": dioIntStatusPort");
    funcArgs = "(mAddress, &data)\n";
    switch (devType) {
    case HAT_ID_MCC_152:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc152_dio_int_status_read_port(address, &data);
        break;
    default:
        funcArgs = "()\n";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        argVals = "(~)";
        break;
    }

    if(argVals == "")
        argVals = QStringLiteral("(%1, %2)")
                .arg(address)
                .arg(data);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    value = data;
    return mResponse;
}

int HatInterface::waitForInterrupt(int timeout)
{
    QString nameOfFunc, funcArgs, funcStr;
    QString argVals;
    QTime t;
    QString sStartTime;
    QString hatName;

    hatName = "Hats";
    nameOfFunc = hatName.append(": waitForInterrupt");
    funcArgs = "(timeout)\n";
    sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
    mResponse = hat_wait_for_interrupt(timeout);

    argVals = QStringLiteral("(%1)")
            .arg(timeout);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    return mResponse;
}

int HatInterface::getInterruptState()
{
    QString nameOfFunc, funcArgs, funcStr;
    QString argVals;
    QTime t;
    QString sStartTime;
    int state;
    QString hatName;

    hatName = "Hats";
    nameOfFunc = hatName.append(": getInterruptState");
    funcArgs = "() = state\n";
    sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
    state = hat_interrupt_state();

    argVals = QString("() = %1").arg(state);
    mStatusString = nameOfFunc + argVals;

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(RESULT_SUCCESS, sStartTime + funcStr);
    return state;
}

int HatInterface::enableCallback(void(*function)(void *), void *userData)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    QString hatName;

    hatName = "Hats";
    nameOfFunc = hatName.append(": callbackEnable");
    funcArgs = "(function)\n";
    sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
    mResponse = hat_interrupt_callback_enable(function, userData);
    argVals = QStringLiteral("(%1)")
            .arg(function != 0);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    return mResponse;
}

int HatInterface::disableCallback()
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    QString hatName;

    hatName = "Hats";
    nameOfFunc = hatName.append(": callbackDisable");
    funcArgs = "()\n";
    sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
    mResponse = hat_interrupt_callback_disable();
    argVals = "()";
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    return mResponse;
}

#else

int HatInterface::getNumAOutChans(uint16_t devType)
{
    (void)devType;
    return 0;
}

int HatInterface::dioReset(uint16_t devType, uint8_t address)
{
    (void)devType;
    (void)address;
    return RESULT_INVALID_DEVICE;
}

int HatInterface::getNumDioChans(uint16_t devType)
{
    (void)devType;
    return 0;
}

int HatInterface::getAOutCodeMax(uint16_t devType)
{
    (void)devType;
    return 0;
}

int HatInterface::getAOutCodeMin(uint16_t devType)
{
    (void)devType;
    return 0;
}

double HatInterface::getAOutVoltsMax(uint16_t devType)
{
    (void)devType;
    return 0.0;
}

double HatInterface::getAOutVoltsMin(uint16_t devType)
{
    (void)devType;
    return 0.0;
}

int HatInterface::aOutWrite(uint16_t devType, uint8_t address, uint8_t chan, uint32_t options, double value)
{
    (void)devType;
    (void)address;
    (void)chan;
    (void)options;
    (void)value;
    return RESULT_INVALID_DEVICE;
}

int HatInterface::aOutWriteAll(uint16_t devType, uint8_t address, uint32_t options, double &values)
{
    (void)devType;
    (void)address;
    (void)options;
    (void)values;
    return RESULT_INVALID_DEVICE;
}

int HatInterface::dioInBitRead(uint16_t devType, uint8_t address, uint8_t chan, uint8_t &value)
{
    (void)devType;
    (void)address;
    (void)chan;
    (void)value;
    return RESULT_INVALID_DEVICE;
}

int HatInterface::dioInPortRead(uint16_t devType, uint8_t address, uint8_t &value)
{
    (void)devType;
    (void)address;
    (void)value;
    return RESULT_INVALID_DEVICE;
}

int HatInterface::dioOutBitWrite(uint16_t devType, uint8_t address, uint8_t chan, uint8_t value)
{
    (void)devType;
    (void)address;
    (void)chan;
    (void)value;
    return RESULT_INVALID_DEVICE;
}

int HatInterface::dioOutPortWrite(uint16_t devType, uint8_t address, uint8_t value)
{
    (void)devType;
    (void)address;
    (void)value;
    return RESULT_INVALID_DEVICE;
}

int HatInterface::dioOutBitRead(uint16_t devType, uint8_t address, uint8_t chan, uint8_t &value)
{
    (void)devType;
    (void)address;
    (void)chan;
    (void)value;
    return RESULT_INVALID_DEVICE;
}

int HatInterface::dioOutPortRead(uint16_t devType, uint8_t address, uint8_t &value)
{
    (void)devType;
    (void)address;
    (void)value;
    return RESULT_INVALID_DEVICE;
}

int HatInterface::dioBitConfigWrite(uint16_t devType, uint8_t address, uint8_t chan, uint8_t item, uint8_t value)
{
    (void)devType;
    (void)address;
    (void)chan;
    (void)item;
    (void)value;
    return RESULT_INVALID_DEVICE;
}

int HatInterface::dioPortConfigWrite(uint16_t devType, uint8_t address, uint8_t item, uint8_t value)
{
    (void)devType;
    (void)address;
    (void)item;
    (void)value;
    return RESULT_INVALID_DEVICE;
}

int HatInterface::dioBitConfigRead(uint16_t devType, uint8_t address, uint8_t chan, uint8_t item, uint8_t &value)
{
    (void)devType;
    (void)address;
    (void)chan;
    (void)item;
    (void)value;
    return RESULT_INVALID_DEVICE;
}

int HatInterface::dioPortConfigRead(uint16_t devType, uint8_t address, uint8_t item, uint8_t &value)
{
    (void)devType;
    (void)address;
    (void)item;
    (void)value;
    return RESULT_INVALID_DEVICE;
}

int HatInterface::dioIntStatusBit(uint16_t devType, uint8_t address, uint8_t chan, uint8_t &value)
{
    (void)devType;
    (void)address;
    (void)chan;
    (void)value;
    return RESULT_INVALID_DEVICE;
}

int HatInterface::dioIntStatusPort(uint16_t devType, uint8_t address, uint8_t &value)
{
    (void)devType;
    (void)address;
    (void)value;
    return RESULT_INVALID_DEVICE;
}

int HatInterface::waitForInterrupt(int timeout)
{
    (void)timeout;
    return RESULT_INVALID_DEVICE;
}

int HatInterface::getInterruptState()
{
    return RESULT_INVALID_DEVICE;
}

int HatInterface::enableCallback(void(*function)(void *), void *userData)
{
    (void)function;
    (void)userData;
    return RESULT_INVALID_DEVICE;
}

int HatInterface::disableCallback()
{
    return RESULT_INVALID_DEVICE;
}

#endif

#ifdef HAT_05

int HatInterface::ainClockConfigWrite(uint16_t devType, uint8_t address, uint8_t source, double rate)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": ainClockConfigWrite");
    funcArgs = "(mAddress, source, rate)\n";
    switch (devType) {
    case HAT_ID_MCC_172:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc172_a_in_clock_config_write(address, source, rate);
        break;
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QStringLiteral("(%1, %2, %3)")
            .arg(address)
            .arg(source)
            .arg(rate);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    return mResponse;
}

int HatInterface::iepeConfigRead(uint16_t devType, uint8_t address, uint8_t channel, uint8_t &value)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    uint8_t data;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": iepeConfigRead");
    funcArgs = "(mAddress, channel, &data)\n";
    switch (devType) {
    case HAT_ID_MCC_172:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc172_iepe_config_read(address, channel, &data);
        break;
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QStringLiteral("(%1, %2, %3)")
            .arg(address)
            .arg(channel)
            .arg(data);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    value = data;
    return mResponse;
}

int HatInterface::iepeConfigWrite(uint16_t devType, uint8_t address, uint8_t channel, uint8_t value)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    QString hatName;

    hatName = getHatTypeName(devType);
    nameOfFunc = hatName.append(": iepeConfigWrite");
    funcArgs = "(mAddress, channel, value)\n";
    switch (devType) {
    case HAT_ID_MCC_172:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc172_iepe_config_write(address, channel, value);
        break;
    default:
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QStringLiteral("(%1, %2, %3)")
            .arg(address)
            .arg(channel)
            .arg(value);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    reportResult(mResponse, sStartTime + funcStr);
    return mResponse;
}

#else

int HatInterface::ainClockConfigWrite(uint16_t devType, uint8_t address, uint8_t source, double rate)
{
    (void)devType;
    (void)address;
    (void)source;
    (void)rate;
    return RESULT_INVALID_DEVICE;
}

int HatInterface::iepeConfigRead(uint16_t devType, uint8_t address, uint8_t channel, uint8_t &value)
{
    (void)devType;
    (void)address;
    (void)channel;
    (void)value;
    return RESULT_INVALID_DEVICE;
}

int HatInterface::iepeConfigWrite(uint16_t devType, uint8_t address, uint8_t channel, uint8_t value)
{
    (void)devType;
    (void)address;
    (void)channel;
    (void)value;
    return RESULT_INVALID_DEVICE;
}
#endif
