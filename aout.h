#ifndef AOUT_H
#define AOUT_H

#include <QWidget>
#include <QTimer>
#include <QtMath>
#include "qcustomplot.h"
#include "unitest.h"
#include "errordialog.h"
#include "testutilities.h"
#include "hatinterface.h"
#include "daqhats/daqhats.h"

namespace Ui {
class AOut;
}

class AOut : public QWidget
{
    Q_OBJECT

public:
    explicit AOut(QWidget *parent = 0);
    ~AOut();
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);

private slots:
    void stopCmdClicked();
    void goCmdClicked();
    void functionChanged(int utFunction);
    void runSelectedFunction();
    void showPlotWindow(bool showIt);
    void updateParameters();
    void initDeviceParams();
    void updateValueBox();
    void valueChanged();
    void showQueueConfig();

private:
    Ui::AOut *ui;
    HatInterface *hatInterface;
    ErrorDialog errDlg;
    QActionGroup *functionGroup;
    QActionGroup *optionGroup;

    int mNumHats;
    int mDevIndex;
    int mResponse;
    int mNumAoChans;
    uint8_t mAddress;
    uint16_t mHatID;
    QString mDevName;

    uint32_t mScanOptions;
    struct HatInfo hatInfoList[MAX_NUMBER_HATS];
    uint32_t mSamplesPerChan;
    QString mOptNames;
    bool mBackgroundScan;

    QMap<int, int> mChanList;
    bool mQueueEnabled;

    uint8_t mChanCount;
    int mAoResolution;
    int mCurFunction;

    long long mTotalRead;
    QString mFuncName;

    bool mUseTimer;
    bool mGoTmrIsRunning;
    bool mStopOnStart;
    bool mOneSampPerForTotalSamps;
    int mTmrInterval;

    bool mPlot;
    bool mRunning;
    int mPlotChan;
    double *buffer;
    QVector<double> xValues;
    QVector<QVector<double>> yChans;
    int32_t mBlockSize;
    long long mBufSize;
    long long mFinalCount;

    void setUiForFunction();
    void runAOutFunc();
    void runAOutScanFunc();
};

#endif // AOUT_H
