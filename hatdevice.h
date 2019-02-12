#ifndef HATDEVICE_H
#define HATDEVICE_H

#include <QWidget>
#include <QTimer>
#include "qcustomplot.h"
#include "queuedialog.h"
#include "unitest.h"
#include "errordialog.h"
#include "testutilities.h"
#include "hatinterface.h"
#include "daqhats/daqhats.h"

namespace Ui {
class HatDevice;
}

class HatDevice : public QWidget
{
    Q_OBJECT

public:
    explicit HatDevice(QWidget *parent = 0);
    ~HatDevice();
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);

private slots:
    void stopCmdClicked();
    void goCmdClicked();
    void functionChanged(int utFunction);
    void runSelectedFunction();
    void checkStatus();
    void replot();
    void plotSelect();
    void showPlotWindow(bool showIt);
    void updateParameters();
    void showQueueConfig();
    void queueDialogResponse();
    void writeStoredTcValues();
    void updateTcValues();
    //void runBackgrndResult();

private:
    Ui::HatDevice *ui;
    HatInterface *hatInterface;
    QTimer *tmrGoTimer;
    QTimer *tmrBGResultRead;
    QRadioButton *rbPlotSel[8]={};
    QActionGroup *functionGroup;
    QActionGroup *optionGroup;
    QActionGroup *trigTypeGroup;
    QueueDialog *queueSetup;
    ErrorDialog errDlg;

    int mNumHats;
    int mDevIndex;
    int mResponse;
    uint8_t mAddress;
    uint16_t mHatID;
    QString mDevName;
    QString mSerNum;

    uint32_t mScanOptions;
    TriggerMode mTriggerType;
    uint32_t mSamplesPerChan;
    double mRateReturned;
    struct HatInfo hatInfoList[MAX_NUMBER_HATS];
    QString mOptNames;
    bool mBackgroundScan;

    QMap<int, int> mChanList;
    bool mQueueEnabled;

    QHash<int, bool> mPlotList;
    uint8_t mChanCount;
    int mAiResolution;
    int mCurFunction;
    long long mPlotIndex;
    long long mPlotCount;
    long long mTotalRead;
    long long mPlotSize;
    QString mFuncName;
    QString mTInPrefs = "";

    bool mUseTimer;
    bool mGoTmrIsRunning;
    bool mStopOnStart;
    bool mOneSampPerForTotalSamps;
    int mTmrInterval;

    bool mPlot;
    bool mRunning;
    bool mTriggered;
    bool mUseGetStatus;
    bool mStatusTimerEnabled;
    int mPlotChan;
    double *buffer;
    double *data;
    QVector<double> xValues;
    QVector<QVector<double>> yChans;
    int32_t mBlockSize;
    long long mBufSize;
    long long mFinalCount;

    QStringList mFuncHistoryList;
    int mHistListSize;

    //void showBoardParameters();
    //void runOpenDevice(uint8_t address);
    //void runCloseDevice(uint8_t address);
    void setUiForFunction();
    void checkCurTcConfig(bool saveToChild);
    void runAinFunction();
    void runTinFunction();
    void runAInScanFunc();
    void runSetTriggerFunc();
    void stopScan();
    void readBuffer();

    void setupPlot(QCustomPlot *dataPlot, int chanCount);
    void printData(unsigned long long currentCount, long long currentIndex, int blockSize);
    void plotScan(unsigned long long currentCount, long long currentIndex, int blockSize);
    void updatePlot();
    void setupQueue();

    //void setError(int curError, QString funcText);
    //void addFunction(QString funcString);
    void runReadScanStatus();
};

#endif // HATDEVICE_H
