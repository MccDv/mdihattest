#ifndef HATINTERFACE_H
#define HATINTERFACE_H

#include <QApplication>
#include <QWidget>
#include <QTime>
#include "mainwindow.h"
#include "daqhats/daqhats.h"

class HatInterface
{
public:
    HatInterface();

    QHash<uint8_t, QString> mHatList;
    QHash<uint8_t, uint16_t> mHatIDList;

    QHash<uint8_t, QString> refreshHatList();
    QHash<uint8_t, uint16_t> refreshHatIDList();
    QString getStatus() { return mStatusString; }
    int openDevice(uint16_t devType, uint8_t address);
    int closeDevice(uint16_t devType, uint8_t address);
    bool deviceIsOpen(uint16_t devType, uint8_t address);
    int getFirmwareVersion(uint16_t devType, uint8_t address, uint16_t &fwMain, uint16_t &fwBoot);
    int getSerialNumber(uint16_t devType, uint8_t address, QString &serialNum);
    int blinkLED(uint16_t devType, uint8_t address, uint8_t count);
    int readCalDate(uint16_t devType, uint8_t address, QString &calDate);
    int getNumAInChans(uint16_t devType);
    int readCalCoeffs(uint16_t devType, uint8_t address, uint8_t chan, double &slope, double &offset);
    int writeCalCoeffs(uint16_t devType, uint8_t address, uint8_t chan, double slope, double offset);
    int aInRead(uint16_t devType, uint8_t address, uint8_t chan, uint32_t options, double &value);
    int setTrigger(uint16_t devType, uint8_t address, TriggerMode trigType);
    int stopAInScan(uint16_t devType, uint8_t address);
    int readAInScanStatus(uint16_t devType, uint8_t address, uint16_t &status, uint32_t &sampsAvailable);
    int aInScanChanCount(uint16_t devType, uint8_t address);
    int aInScanCleanup(uint16_t devType, uint8_t address);
    int testClock(uint16_t devType, uint8_t address, uint8_t mode, uint8_t &value);
    int testTrig(uint16_t devType, uint8_t address, uint8_t &value);

    int tInRead(uint16_t devType, uint8_t address, uint8_t chan, double &temp);
    int boardTemp(uint16_t devType, uint8_t address, double &temp);
    int readTcTypes(uint16_t devType, uint8_t address, uint8_t chan, uint8_t &tcType);
    int writeTcType(uint16_t devType, uint8_t address, uint8_t chan, uint8_t tcType);

    int getNumAOutChans(uint16_t devType);
    int getNumDioChans(uint16_t devType);
    int aOutWrite(uint16_t devType, uint8_t address, uint8_t chan, uint32_t options, double value);
    int aOutWriteAll(uint16_t devType, uint8_t address, uint32_t options, double &values);
    int dioReset(uint16_t devType, uint8_t address);
    int dioInputRead(uint16_t devType, uint8_t address, uint8_t chan, uint8_t &value);
    int dioOutputWrite(uint16_t devType, uint8_t address, uint8_t chan, uint8_t value);
    int dioOutputRead(uint16_t devType, uint8_t address, uint8_t chan, uint8_t &value);
    int dioDirectionWrite(uint16_t devType, uint8_t address, uint8_t chan, uint8_t value);
    int dioDirectionRead(uint16_t devType, uint8_t address, uint8_t chan, uint8_t &value);
    int dioPullConfigWrite(uint16_t devType, uint8_t address, uint8_t chan, uint8_t value);
    int dioPullConfigRead(uint16_t devType, uint8_t address, uint8_t chan, uint8_t &value);
    int dioPullEnableWrite(uint16_t devType, uint8_t address, uint8_t chan, uint8_t value);
    int dioPullEnableRead(uint16_t devType, uint8_t address, uint8_t chan, uint8_t &value);
    int dioInputInvertWrite(uint16_t devType, uint8_t address, uint8_t chan, uint8_t value);
    int dioInputInvertRead(uint16_t devType, uint8_t address, uint8_t chan, uint8_t &value);
    int dioInputLatchWrite(uint16_t devType, uint8_t address, uint8_t chan, uint8_t value);
    int dioInputLatchRead(uint16_t devType, uint8_t address, uint8_t chan, uint8_t &value);
    int dioOutputTypeWrite(uint16_t devType, uint8_t address, uint8_t value);
    int dioOutputTypeRead(uint16_t devType, uint8_t address, uint8_t &value);
    int dioInterruptMaskWrite(uint16_t devType, uint8_t address, uint8_t chan, uint8_t value);
    int dioInterruptMaskRead(uint16_t devType, uint8_t address, uint8_t chan, uint8_t &value);
    int dioInterruptStatusRead(uint16_t devType, uint8_t address, uint8_t chan, uint8_t &value);

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
