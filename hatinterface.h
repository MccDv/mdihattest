#ifndef HATINTERFACE_H
#define HATINTERFACE_H

#define OPTS_IFC_BACKGROUND         (0x1000)

//#include "miscform.h"

#include <QApplication>
#include <QWidget>
#include <QTime>
#include "mainwindow.h"
#include "daqhats/daqhats.h"

#ifndef HAT_03

#define TC_DISABLED  (0xFF)

#endif

class HatInterface
{
public:
    HatInterface();

    QHash<uint8_t, QString> mHatList;
    QHash<uint8_t, uint16_t> mHatIDList;

    QHash<uint8_t, QString> refreshHatList();
    QHash<uint8_t, uint16_t> refreshHatIDList();
    QString getStatus() { return mStatusString; }
    QString getErrorMessage(int result);
    int openDevice(uint16_t devType, uint8_t address);
    int closeDevice(uint16_t devType, uint8_t address);
    bool deviceIsOpen(uint16_t devType, uint8_t address);
    int getFirmwareVersion(uint16_t devType, uint8_t address, uint16_t &fwMain, uint16_t &fwBoot);
    int getSerialNumber(uint16_t devType, uint8_t address, QString &serialNum);
    int blinkLED(uint16_t devType, uint8_t address, uint8_t count);
    int readCalDate(uint16_t devType, uint8_t address, QString &calDate);
    int getNumAInChans(uint16_t devType);
    double getAInVoltsMax(uint16_t devType);
    double getAInVoltsMin(uint16_t devType);
    double getAInRangeMax(uint16_t devType);
    double getAInRangeMin(uint16_t devType);
    uint16_t getAInCodeMax(uint16_t devType);
    uint16_t getAInCodeMin(uint16_t devType);
    int readCalCoeffs(uint16_t devType, uint8_t address, uint8_t chan, double &slope, double &offset);
    int writeCalCoeffs(uint16_t devType, uint8_t address, uint8_t chan, double slope, double offset);
    int aInRead(uint16_t devType, uint8_t address, uint8_t chan, uint32_t options, double &value);
    int setTrigger(uint16_t devType, uint8_t address, uint8_t source, TriggerMode trigType);
    int stopAInScan(uint16_t devType, uint8_t address);
    int getBufferSize(uint16_t devType, uint8_t address, uint32_t &bufferSize);
    int readAInScanStatus(uint16_t devType, uint8_t address, uint16_t &status, uint32_t &sampsAvailable);
    int aInScanChanCount(uint16_t devType, uint8_t address);
    int aInScanCleanup(uint16_t devType, uint8_t address);
    int testClock(uint16_t devType, uint8_t address, uint8_t mode, uint8_t &value);
    int testTrig(uint16_t devType, uint8_t address, uint8_t &value);

    int tInRead(uint16_t devType, uint8_t address, uint8_t chan, double &temp);
    int tInReadBackground(uint16_t devType, uint8_t address, uint8_t chanMask);
    //int backgroundResult(uint16_t devType, uint8_t address, uint8_t *chanMask, uint8_t *count, double &temp);
    int boardTemp(uint16_t devType, uint8_t address, uint8_t chan, double &temp);
    int readTcTypes(uint16_t devType, uint8_t address, uint8_t chan, uint8_t &tcType);
    int writeTcType(uint16_t devType, uint8_t address, uint8_t chan, uint8_t tcType);
    int readInterval(uint16_t devType, uint8_t address, uint8_t &interval);
    int writeInterval(uint16_t devType, uint8_t address, uint8_t interval);

    double getAOutVoltsMax(uint16_t devType);
    double getAOutVoltsMin(uint16_t devType);
    double getAOutRangeMax(uint16_t devType);
    double getAOutRangeMin(uint16_t devType);
    int getAOutCodeMax(uint16_t devType);
    int getAOutCodeMin(uint16_t devType);
    int getNumAOutChans(uint16_t devType);
    int getNumDioChans(uint16_t devType);
    int aOutWrite(uint16_t devType, uint8_t address, uint8_t chan, uint32_t options, double value);
    int aOutWriteAll(uint16_t devType, uint8_t address, uint32_t options, double &values);
    int dioReset(uint16_t devType, uint8_t address);
    int dioInBitRead(uint16_t devType, uint8_t address, uint8_t chan, uint8_t &value);
    int dioInPortRead(uint16_t devType, uint8_t address, uint8_t &value);
    int dioOutBitWrite(uint16_t devType, uint8_t address, uint8_t chan, uint8_t value);
    int dioOutPortWrite(uint16_t devType, uint8_t address, uint8_t value);
    int dioOutBitRead(uint16_t devType, uint8_t address, uint8_t chan, uint8_t &value);
    int dioOutPortRead(uint16_t devType, uint8_t address, uint8_t &value);
    int dioBitConfigWrite(uint16_t devType, uint8_t address, uint8_t chan, uint8_t item, uint8_t value);
    int dioPortConfigWrite(uint16_t devType, uint8_t address, uint8_t item, uint8_t value);
    int dioBitConfigRead(uint16_t devType, uint8_t address, uint8_t chan, uint8_t item, uint8_t &value);
    int dioPortConfigRead(uint16_t devType, uint8_t address, uint8_t item, uint8_t &value);
    int dioIntStatusBit(uint16_t devType, uint8_t address, uint8_t chan, uint8_t &value);
    int dioIntStatusPort(uint16_t devType, uint8_t address, uint8_t &value);

    int ainClockConfigRead(uint16_t devType, uint8_t address, uint8_t &source, double &rate, uint8_t &value);
    int ainClockConfigWrite(uint16_t devType, uint8_t address, uint8_t source, double rate);
    int iepeConfigRead(uint16_t devType, uint8_t address, uint8_t channel, uint8_t &value);
    int iepeConfigWrite(uint16_t devType, uint8_t address, uint8_t channel, uint8_t value);

    int waitForInterrupt(int timeout);
    int getInterruptState();
    int enableCallback(void(*function)(void *), void *userData);
    int disableCallback();

    void addToMenu(uint16_t devType, uint8_t address, QString devName);
    void removeFromMenu(uint8_t address);
    void reportResult(int response, QString description);
    void updateAppStatus(UtFunctionGroup funcGroup, QString statusText);

private:
    MainWindow *mMainWindow;
    MainWindow* getMainWindow();
    int mResponse;
    QString mStatusString;

};


#endif // HATINTERFACE_H
