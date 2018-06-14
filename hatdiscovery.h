#ifndef HATDISCOVERY_H
#define HATDISCOVERY_H

#include <QWidget>
#include <QTime>
#include "hatinterface.h"
#include "daqhats/daqhats.h"

namespace Ui {
class HatDiscovery;
}

class HatDiscovery : public QWidget
{
    Q_OBJECT

private slots:
    void readHatList();
    void devSelectedChanged();
    void functionChanged(int utFunction);
    void showPlotWindow(bool showIt);
    void openCmdClicked();
    void closeCmdClicked();
    void updateParameters();
    void showQueueConfig();

public:
    explicit HatDiscovery(QWidget *parent = 0);
    ~HatDiscovery();

private:
    Ui::HatDiscovery *ui;
    HatInterface *hatInterface;

    int mNumHats;
    int mDevIndex;
    int mResponse;
    uint8_t mAddress;
    struct HatInfo hatInfoList[MAX_NUMBER_HATS];
    int mUtFunction;
    bool mShowPlot;

    void showBoardParameters();
    void runOpenDevice(uint8_t address);
    void runCloseDevice(uint8_t address);
};

#endif // HATDISCOVERY_H
