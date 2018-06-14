#include "testutilities.h"

void delay(int milliSeconds)
{
    QTime dieTime= QTime::currentTime().addMSecs(milliSeconds);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
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
        optString = optString.left(optString.length() - 2);
    }
    return optString;
}

QString getStatusText(uint16_t status)
{
    QString statusString;

    statusString = "";
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
    default:
        return "unknown";
        break;
    }
}
#endif
