#ifndef TESTUTILITIES_H
#define TESTUTILITIES_H

#include <QtCore>
#include "unitest.h"
#include "daqhats/daqhats.h"

void delay(int milliSeconds);
double getVoltsFromResolution(int resolution, double rangeVoltsMin, double rangeVoltsMax, int counts);
double getVoltsFromCounts(int maxCode, double rangeVoltsMin, double rangeVoltsMax, int counts);
QString getFuncGroupName(UtFunctionGroup funcGroup);
QString getOptionNames(uint32_t curOptions);
QString getErrorDescription(int response);
QString getStatusText(uint16_t status);
QString getTrigText(TriggerMode trigType);
QString getSourceText(uint8_t sourceClockTrig);
QString getTcTypeName(uint8_t tcType);
QString getConfigItemName(uint8_t configItem);
QString getHatTypeName(uint16_t hatType);
QString setTcTypeInString(QString tcTypes, uint8_t chan, uint8_t newType);
uint16_t getHatIdFromName(QString hatName);
QVector<uint8_t> getTcTypesFromString(QString tcTypes);
#endif // TESTUTILITIES_H

#ifndef HAT_03

#define HAT_ID_MCC_134       0x0143

#define TC_TYPE_J   0
#define TC_TYPE_K   1
#define TC_TYPE_T   2
#define TC_TYPE_E   3
#define TC_TYPE_R   4
#define TC_TYPE_S   5
#define TC_TYPE_B   6
#define TC_TYPE_N   7

#endif

#ifndef HAT_04

#define HAT_ID_MCC_152       0x0144

    /// Configure channel direction
#define DIO_DIRECTION        0
    /// Configure pull-up/down resistor
#define DIO_PULL_CONFIG      1
    /// Enable pull-up/down resistor
#define DIO_PULL_ENABLE      2
    /// Configure input inversion
#define DIO_INPUT_INVERT     3
    /// Configure input latching
#define DIO_INPUT_LATCH      4
    /// Configure output type
#define DIO_OUTPUT_TYPE      5
    /// Configure interrupt mask
#define DIO_INT_MASK         6

#endif
