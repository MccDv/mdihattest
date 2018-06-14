#ifndef TESTUTILITIES_H
#define TESTUTILITIES_H

#include <QtCore>
#include "daqhats/daqhats.h"

void delay(int milliSeconds);
QString getOptionNames(uint32_t curOptions);
QString getErrorDescription(int response);
QString getStatusText(uint16_t status);
QString getTrigText(TriggerMode trigType);
QString getTcTypeName(uint8_t tcType);

#endif // TESTUTILITIES_H
