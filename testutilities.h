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
QString getRangeText(uint8_t range);
QString getModeText(uint8_t mode);
QString getTcTypeName(uint8_t tcType);
QString getConfigItemName(uint8_t configItem);
QString getHatTypeName(uint16_t hatType);
QString setTcTypeInString(QString tcTypes, uint8_t chan, uint8_t newType);
uint16_t getHatIdFromName(QString hatName);
QVector<uint8_t> getTcTypesFromString(QString tcTypes);

#endif // TESTUTILITIES_H
