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
    void setInterval();
    void trapVoltsChecked();
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

    int mNumHats = 0;
    int mDevIndex = 0;
    int mResponse = 0;
    uint8_t mAddress = 0;
    uint16_t mHatID = 0;
    QString mDevName = "";
    QString mSerNum = "";

    uint32_t mScanOptions = 0;
    TriggerMode mTriggerType = TRIG_RISING_EDGE;
    uint8_t mTriggerSource = SOURCE_LOCAL;
    uint8_t mRange = AI_RANGE_BIP_10V;
    uint32_t mSamplesPerChan = 0;
    double mRateReturned = 0.0;
    struct HatInfo hatInfoList[MAX_NUMBER_HATS];
    QString mOptNames = "";
    bool mBackgroundScan = true;
    int32_t maInMinCode;
    int32_t maInMaxCode;
    double maInMaxRange;
    double maInMinRange;

    QMap<int, int> mChanList;
    bool mQueueEnabled = false;

    QHash<int, bool> mPlotList;
    uint8_t mChanCount = 0;
    int mAiResolution = 0;
    int mCurFunction = 0;
    long long mPlotInde = 0;
    long long mPlotCount = 0;
    long long mTotalRead = 0;
    long long mPlotSize = 0;
    int mTextIndex = 0;
    QString mFuncName = "";
    QString mTInPrefs = "";

    bool mUseTimer = false;
    bool mTimerConfigured = false;
    bool mGoTmrIsRunning = false;
    bool mStopOnStart = false;
    bool mOneSampPerForTotalSamps = false;
    int mTmrInterval = 1000;

    bool mPlot = false;
    bool mRunning = false;
    bool mTriggered = false;
    bool mAbort = false;
    bool mUseGetStatus = true;
    bool mStatusTimerEnabled = false;
    int mPlotChan = 0;
    double *buffer;
    double *data;
    QVector<double> xValues;
    QVector<QVector<double>> yChans;
    int32_t mBlockSize = 0;
    long long mBufSize = 0;
    long long mFinalCount = 0;
    double mTimeout = 10;
    double mTrapVal = 1.5;
    bool mHaltAction = false;
    long mTimerIteration = 0;

    QStringList mFuncHistoryList;
    int mHistListSize = 50;

    //void showBoardParameters();
    //void runOpenDevice(uint8_t address);
    //void runCloseDevice(uint8_t address);
    void setUiForFunction();
    void checkCurTcConfig(bool saveToChild);
    void runAinFunction();
    void runTinFunction();
    void runAInScanFunc();
    void runAInScan172Func();
    void runSetTriggerFunc();
    void stopScan();
    void readBuffer();

    void setupPlot(QCustomPlot *dataPlot, int chanCount);
    void printData(unsigned long long currentCount, long long currentIndex, int blockSize);
    void plotScan(unsigned long long currentCount, long long currentIndex, int blockSize);
    void updatePlot();
    void updateData();
    void setupQueue();

    //void setError(int curError, QString funcText);
    //void addFunction(QString funcString);
    void runReadScanStatus();
};

#endif // HATDEVICE_H
