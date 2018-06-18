#ifndef TESTUTILITIES_H
#define TESTUTILITIES_H

#include <QtCore>
#include "daqhats/daqhats.h"

#ifdef HAT_ID_MCC_118
#define HAT_TYPE_118 HAT_ID_MCC_118
#else
#define HAT_TYPE_118  0
#endif

#ifdef HAT_ID_MCC_134
#define HAT_TYPE_134 HAT_ID_MCC_134
#else
#define HAT_TYPE_134 0
#endif

#ifdef HAT_ID_MCC_152
#define HAT_TYPE_152 HAT_ID_MCC_152
#else
#define HAT_TYPE_152 0
#endif

void delay(int milliSeconds);
QString getOptionNames(uint32_t curOptions);
QString getErrorDescription(int response);
QString getStatusText(uint16_t status);
QString getTrigText(TriggerMode trigType);
QString getTcTypeName(uint8_t tcType);
QString getHatTypeName(uint16_t hatType);
#endif // TESTUTILITIES_H
