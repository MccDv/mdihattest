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
    int readTcTypes(uint16_t devType, uint8_t address, uint8_t chan, uint8_t &tcType);
    int writeTcType(uint16_t devType, uint8_t address, uint8_t chan, uint8_t tcType);
    int aInRead(uint16_t devType, uint8_t address, uint8_t chan, uint32_t options, double &value);
    int tInRead(uint16_t devType, uint8_t address, uint8_t chan, double &temp);
    int vInRead(uint16_t devType, uint8_t address, uint8_t chan, double &temp);
    int boardTemp(uint16_t devType, uint8_t address, double &temp);
    int setTrigger(uint16_t devType, uint8_t address, TriggerMode trigType);
    int stopAInScan(uint16_t devType, uint8_t address);
    int readAInScanStatus(uint16_t devType, uint8_t address, uint16_t &status, uint32_t &sampsAvailable);
    int aInScanChanCount(uint16_t devType, uint8_t address);
    int aInScanCleanup(uint16_t devType, uint8_t address);
    int testClock(uint16_t devType, uint8_t address, uint8_t mode, uint8_t &value);
    int testTrig(uint16_t devType, uint8_t address, uint8_t &value);

    void addToMenu(uint16_t devType, uint8_t address, QString devName);
    void removeFromMenu(uint8_t address);
    void reportResult(int response, QString description);

private:
    MainWindow *mMainWindow;
    MainWindow* getMainWindow();
    int mResponse;
    QString mStatusString;

};


#endif // HATINTERFACE_H
