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
    void devSelectedChanged();
    void functionChanged(int utFunction);
    void runSelectedFunction();
    void showBoardParameters();
    void showPlotWindow(bool showIt);
    void updateParameters();
    void showQueueConfig();
    void readCalClicked();
    void loadCalClicked();
    //void flashLEDClicked();
    void showErrorMessage();

private:
#define READ_CAL            1
#define WRITE_CAL           2
#define READ_TC_TYPES       3
#define WRITE_TC_TYPE       4
#define FLASH_LED           5
#define CLOCK_TEST          6
#define NUM_SCAN_CHANS      7
#define READ_STATUS         8
#define READ_ERROR          9
#define READ_INT_STAT       10
#define READ_SCAN_PARAMS    11
#define WRITE_SCAN_CLEAN    12
#define READ_IEPE_CONFIG    13
#define WRITE_IEPE_CONFIG   14


    Ui::InfoForm *ui;
    HatInterface *hatInterface;
    QHash<uint8_t, QString> mHatList;
    QHash<uint8_t, uint16_t> mHatIDList;

    int mNumHats;
    QString mDevName;
    QString mSerNum;
    int mResponse;
    uint8_t mAddress;
    uint16_t mHatID;
    int mUtFunction;
    int mSelectedFunction;
    bool mShowPlot;
    QString mTInPrefs;

    void readCal();
    void writeCal();
    void writeTcType();
    void readTcTypes();
    void readClkTrg();
    void readNumScanChans();
    void flashLED();
    void readStatus();
    void readIntStatus();
    void readScanParams();
    void runCleanScan();
    void readStoredTypes();
    void readIEPEConfig();
    void writeIEPEConfig();

};

#endif // INFOFORM_H
