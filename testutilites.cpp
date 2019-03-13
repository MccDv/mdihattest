#include "testutilities.h"
#include "hatinterface.h"

void delay(int milliSeconds)
{
    QTime dieTime= QTime::currentTime().addMSecs(milliSeconds);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

double getVoltsFromResolution(int resolution, double rangeVoltsMin, double rangeVoltsMax, int counts)
{
    double LSB, bipOffset, calcVolts;

    bipOffset = 0;
    double FSR = rangeVoltsMax - rangeVoltsMin;
    LSB = FSR / qPow(2, resolution);
    if (rangeVoltsMin != 0)
        bipOffset = rangeVoltsMax / 2;
    calcVolts = counts * LSB;
    return calcVolts - bipOffset;
}

double getVoltsFromCounts(int maxCode, double voltsMin, double voltsMax, int counts)
{
    double LSB, bipOffset, calcVolts;

    bipOffset = 0;
    double FSR = voltsMax - voltsMin;
    LSB = FSR / maxCode;
    if (voltsMin != 0)
        bipOffset = voltsMax / 2;
    calcVolts = counts * LSB;
    return calcVolts - bipOffset;
}

QString getHatTypeName(uint16_t hatType)
{
    switch (hatType) {
    case HAT_ID_MCC_118:
        return "MCC118";
        break;
    case HAT_ID_MCC_134:
        return "MCC134";
        break;
    case HAT_ID_MCC_152:
        return "MCC152";
        break;
    default:
        return "NO_ID";
        break;
    }
}

uint16_t getHatIdFromName(QString hatName)
{
    if(hatName == "MCC118")
        return HAT_ID_MCC_118;
#ifdef HAT_03
    if(hatName == "MCC134")
        return HAT_ID_MCC_134;
#endif
#ifdef HAT_04
    if(hatName == "MCC152")
        return HAT_ID_MCC_152;
#endif
    return 0;
}

QString getFuncGroupName(UtFunctionGroup funcGroup)
{
    switch (funcGroup) {
    case FUNC_GROUP_DISC:
        return "Discovery Functions";
        break;
    case FUNC_GROUP_AIN:
        return "Analog Input";
        break;
    case FUNC_GROUP_AOUT:
        return "Analog Output";
        break;
    case FUNC_GROUP_DIN:
        return "Digital Input";
        break;
    case FUNC_GROUP_DOUT:
        return "Digital Output";
        break;
    case FUNC_GROUP_CTR:
        return "Counter Functions";
        break;
    case FUNC_GROUP_MISC:
        return "Miscellaneous Functions";
        break;
    case FUNC_GROUP_CONFIG:
        return "Configuration";
        break;
    default:
        return "Invalid function group";
        break;
    }

}

QString getOptionNames(uint32_t curOptions)
{
    QString optString;

    if (curOptions == 0)
        optString = "DEFAULT";
    else {
        if (curOptions & OPTS_NOSCALEDATA)
            optString += "NOSCALEDATA, ";
        if (curOptions & OPTS_NOCALIBRATEDATA)
            optString += "NOCALIBRATEDATA, ";
        if (curOptions & OPTS_EXTCLOCK)
            optString += "EXTCLOCK, ";
        if (curOptions & OPTS_EXTTRIGGER)
            optString += "EXTTRIGGER, ";
        if (curOptions & OPTS_CONTINUOUS)
            optString += "CONTINUOUS, ";
        if (curOptions & OPTS_IFC_BACKGROUND)
            optString += "BACKGROUND, ";
        optString = optString.left(optString.length() - 2);
    }
    return optString;
}

QString getStatusText(uint16_t status)
{
    QString statusString;

    statusString = "";
    if(status == 0)
        return "Unavailable";
    if (status & STATUS_HW_OVERRUN)
        statusString += "HW Overrun, ";
    if (status & STATUS_BUFFER_OVERRUN)
        statusString += "Buf Overrun, ";
    if (status & STATUS_TRIGGERED)
        statusString += "Triggered, ";
    if (status & STATUS_RUNNING)
        statusString += "Running, ";
    statusString = statusString.left(statusString.length() - 2);
    return statusString;
}

QString getErrorDescription(int response)
{
    switch (response) {
    case RESULT_SUCCESS:
        return "SUCCESS (No error)";
        break;
    case RESULT_BAD_PARAMETER:
        return "BAD_PARAMETER (Invalid parameter)";
        break;
    case RESULT_BUSY:
        return "BUSY (Device is busy)";
        break;
    case RESULT_TIMEOUT:
        return "TIMEOUT (Resource access timeout)";
        break;
    case RESULT_LOCK_TIMEOUT:
        return "LOCK_TIMEOUT (Resource lock timeout)";
        break;
    case RESULT_INVALID_DEVICE:
        return "INVALID_DEVICE (Wrong device type at address)";
        break;
    case RESULT_RESOURCE_UNAVAIL:
        return "RESOURCE_UNAVAIL (Resource unavailable)";
        break;
    case RESULT_UNDEFINED:
        return "UNDEFINED (Some other error -10)";
        break;
    default:
        return QString("Invalid error code (%1)").arg(response);
        break;
    }
}

QString getTrigText(TriggerMode trigType)
{
    switch (trigType) {
    case TRIG_RISING_EDGE:
        return "RisingEdge";
        break;
    case TRIG_FALLING_EDGE:
        return "FallingEdge";
        break;
    case TRIG_ACTIVE_HIGH:
        return "ActiveHigh";
        break;
    case TRIG_ACTIVE_LOW:
        return "ActiveLow";
        break;
    default:
        return "";
    }
}

#ifdef HAT_03

QString getTcTypeName(uint8_t tcType)
{
    switch (tcType) {
    case TC_TYPE_J:
        return "J";
        break;
    case TC_TYPE_K:
        return "K";
        break;
    case TC_TYPE_T:
        return "T";
        break;
    case TC_TYPE_E:
        return "E";
        break;
    case TC_TYPE_R:
        return "R";
        break;
    case TC_TYPE_S:
        return "S";
        break;
    case TC_TYPE_B:
        return "B";
        break;
    case TC_TYPE_N:
        return "N";
        break;
    case TC_DISABLED:
        return "Disabled";
        break;
    default:
        return "unknown";
        break;
    }
}

QVector <uint8_t> getTcTypesFromString(QString tcTypes)
{
    QString  chanString;
    int nextIndex, prevIndex;
    QVector<uint8_t> tcTypeVector;

    nextIndex = 0;
    prevIndex = 0;
    chanString = "";
    do {
        nextIndex = tcTypes.indexOf(",", nextIndex);
        chanString = tcTypes.mid(prevIndex, nextIndex - prevIndex);
        nextIndex++;
        prevIndex = nextIndex;
        tcTypeVector.push_back(chanString.toUInt());
    } while (nextIndex > 0);
    return tcTypeVector;
}

QString setTcTypeInString(QString tcTypes, uint8_t chan, uint8_t newType)
{
    QString typePrefs;
    QString newTypeString;
    int nextIndex, prevIndex, chanIndex;
    int numChars;

    nextIndex = 0;
    prevIndex = 0;
    chanIndex = 0;
    numChars = 1;
    //chanString = "";
    typePrefs = tcTypes;
    newTypeString = QString("%1").arg(newType);
    do {
        nextIndex = tcTypes.indexOf(",", nextIndex);
        //chanString = tcTypes.mid(prevIndex, nextIndex - prevIndex);
        if(chan == chanIndex) {
            numChars = (nextIndex - prevIndex);
            if (numChars < 0)
                numChars = 255;
            typePrefs.replace(prevIndex, numChars, newTypeString);
            break;
        } else {
            nextIndex++;
            prevIndex = nextIndex;
            nextIndex = tcTypes.indexOf(",", nextIndex);
        }
        chanIndex ++;
    } while (chanIndex <= chan);
    return typePrefs;
}

#else

QString getTcTypeName(uint8_t tcType)
{
    (void)tcType;
    return "";
}

QVector <uint8_t> getTcTypesFromString(QString tcTypes)
{
    (void)TcTypes;
    return NULL;
}

QString setTcTypeInString(QString tcTypes, uint8_t chan, uint8_t newType)
{
    (void)TcTypes;
    (void)chan;
    (void)newType;
    return "";
}

#endif

#ifdef HAT_04

QString getConfigItemName(uint8_t configItem)
{
    switch (configItem) {
    case DIO_DIRECTION:
        return "DIRECTION";
        break;
    case DIO_PULL_CONFIG:
        return "PULL_CONFIG";
        break;
    case DIO_PULL_ENABLE:
        return "PULL_ENABLE";
        break;
    case DIO_INPUT_INVERT:
        return "INPUT_INVERT";
        break;
    case DIO_INPUT_LATCH:
        return "INPUT_LATCH";
        break;
    case DIO_OUTPUT_TYPE:
        return "OUTPUT_TYPE";
        break;
    case DIO_INT_MASK:
        return "INT_MASK";
        break;
    default:
        return "INVALID";
        break;
    }
}

#endif
