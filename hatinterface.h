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

    QString getStatus() { return mStatusString; }
    int openDevice(uint16_t devType, uint8_t address);
    int closeDevice(uint16_t devType, uint8_t address);
    bool deviceIsOpen(uint16_t devType, uint8_t address);
    int getFirmwareVersion(uint16_t devType, uint8_t address, uint16_t &fwMain, uint16_t &fwBoot);
    int getSerialNumber(uint16_t devType, uint8_t address, QString &serialNum);
    int stopAInScan(uint16_t devType, uint8_t address);
    int aInScanCleanup(uint16_t devType, uint8_t address);

private:
    MainWindow *mMainWindow;
    MainWindow* getMainWindow();
    int mResponse;
    QString mStatusString;

};


#endif // HATINTERFACE_H
