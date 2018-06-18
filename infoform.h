#ifndef INFOFORM_H
#define INFOFORM_H

#include <QWidget>
#include <QFile>
#include <QTime>
#include <QTextStream>
#include "hatinterface.h"
#include "daqhats/daqhats.h"

namespace Ui {
class InfoForm;
}

class InfoForm : public QWidget
{
    Q_OBJECT

public:
    explicit InfoForm(QWidget *parent = 0);
    ~InfoForm();

private slots:
    void showSysInfo();
    void findHats();
    void devSelectedChanged(QString);
    void functionChanged(int utFunction);
    void runSelectedFunction();
    void showBoardParameters();
    void showPlotWindow(bool showIt);
    void updateParameters();
    void showQueueConfig();
    void readCalClicked();
    void loadCalClicked();
    void flashLEDClicked();
    void cleanScanClicked();

private:
#define READ_CAL        1
#define WRITE_CAL       2
#define READ_TC_TYPES   3
#define WRITE_TC_TYPE   4
#define FLASH_LED       5
#define CLOCK_TEST      6
#define NUM_SCAN_CHANS  7
#define READ_STATUS     8

    Ui::InfoForm *ui;
    HatInterface *hatInterface;
    QHash<uint8_t, QString> mHatList;
    QHash<uint8_t, uint16_t> mHatIDList;

    int mNumHats;
    QString mDevName;
    int mResponse;
    uint8_t mAddress;
    uint16_t mHatID;
    int mUtFunction;
    int mSelectedFunction;
    bool mShowPlot;

    void readCal();
    void writeCal();
    void writeTcType();
    void readTcTypes();
    void readClkTrg();
    void readNumScanChans();
    void flashLED();
    void readStatus();


};

#endif // INFOFORM_H
