#ifndef HATINTERFACE_H
#define HATINTERFACE_H

#include <QApplication>
#include <QWidget>
#include <QTime>
#include "mainwindow.h"
#include "mcchats/mcchats.h"

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
    int aInRead(uint16_t devType, uint8_t address, uint8_t chan, uint32_t options, double &value);
    int stopAInScan(uint16_t devType, uint8_t address);
    int aInScanCleanup(uint16_t devType, uint8_t address);

    void addToMenu(uint16_t devType, uint8_t address, QString devName);
    void removeFromMenu(uint8_t address);

private:
    MainWindow *mMainWindow;
    MainWindow* getMainWindow();
    int mResponse;
    QString mStatusString;

};


#endif // HATINTERFACE_H
