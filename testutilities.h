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
QString getTcTypeName(uint8_t tcType);
QString getHatTypeName(uint16_t hatType);
uint16_t getHatIdFromName(QString hatName);
#endif // TESTUTILITIES_H
