#ifndef CHILDWINDOW_H
#define CHILDWINDOW_H

#include <QtGui>
#include <QWidget>
#include <QMdiSubWindow>
#include "unitest.h"
#include "tmrdialog.h"
#include "daqhats/daqhats.h"

namespace Ui {
    class ChildWindow;
}

class ChildWindow : public QMdiSubWindow
{
    Q_OBJECT
    Q_PROPERTY(QString devName READ devName WRITE setDevName NOTIFY devNameChanged)
    Q_PROPERTY(QString serNum READ serNum WRITE setSerNum NOTIFY serNumChanged)
    Q_PROPERTY(uint8_t devAddress READ devAddress WRITE setDevAddress NOTIFY devAddressChanged)
    Q_PROPERTY(uint16_t devId READ devId WRITE setDevId NOTIFY devIdChanged)
    Q_PROPERTY(bool showPlot READ showPlot WRITE setShowPlot NOTIFY showPlotChanged)
    Q_PROPERTY(u_int32_t scanOptions READ scanOptions WRITE setScanOptions NOTIFY scanOptionsChanged)
    Q_PROPERTY(int curFunction READ curFunction WRITE setCurFunction NOTIFY curFunctionChanged)
    Q_PROPERTY(TriggerMode triggerType READ triggerType WRITE setTriggerType NOTIFY triggerTypeChanged)
    Q_PROPERTY(uint8_t triggerSource READ triggerSource WRITE setTriggerSource NOTIFY triggerSourceChanged)
    Q_PROPERTY(uint8_t aiRange READ aiRange WRITE setAiRange NOTIFY aiRangeChanged)

    Q_PROPERTY(bool tmrEnabled READ tmrEnabled WRITE setTmrEnabled NOTIFY tmrEnabledChanged)
    Q_PROPERTY(int tmrInterval READ tmrInterval WRITE setTmrInterval NOTIFY tmrIntervalChanged)
    Q_PROPERTY(bool tmrSampPerInterval READ tmrSampPerInterval WRITE setTmrSampPerInterval NOTIFY tmrSampPerIntervalChanged)
    Q_PROPERTY(bool tmrStopOnStart READ tmrStopOnStart WRITE setTmrStopOnStart NOTIFY tmrStopOnStartChanged)
    Q_PROPERTY(bool tmrRunning READ tmrRunning WRITE setTmrRunning NOTIFY tmrRunningChanged)
    Q_PROPERTY(QString tInPrefs READ tInPrefs WRITE setTInPrefs NOTIFY tInPrefsChanged)

public:
    explicit ChildWindow(QWidget *parent = 0, UtFunctionGroup funcGroup = FUNC_GROUP_DISC);
    ~ChildWindow();
    void closeEvent(QCloseEvent *event);

    void setDevName(QString devName)
    {
        mDevName = devName;
        emit devNameChanged(devName);
    }

    void setSerNum(QString serNum)
    {
        mSerNum = serNum;
        emit serNumChanged(serNum);
    }

    void setDevAddress(uint8_t devAddress)
    {
        mDevAddress = devAddress;
        emit devAddressChanged(devAddress);
    }

    void setDevId(uint16_t devID)
    {
        mDevID = devID;
        emit devIdChanged(devID);
    }

    void setAiRange(uint8_t aiRange)
    {
        mRange = aiRange;
        emit aiRangeChanged(aiRange);
    }

    void setCurFunction(int utFunction)
    {
        mCurFunction = utFunction;
        emit curFunctionChanged(utFunction);
    }

    void setScanOptions(u_int32_t scanOptions)
    {
        mScanOptions = scanOptions;
        emit scanOptionsChanged(scanOptions);
    }

    void setShowPlot(bool showIt)
    {
        mShowPlot = showIt;
        emit showPlotChanged(showIt);
    }

    void setTriggerType(TriggerMode trigType)
    {
        mTriggerType = trigType;
        emit triggerTypeChanged(trigType);
    }

    void setTriggerSource(uint8_t trigSource)
    {
        mTriggerSource = trigSource;
        emit triggerSourceChanged(trigSource);
    }

    void setTmrEnabled(bool tmrEnabled)
    {
        mTmrEnabled = tmrEnabled;
        emit tmrEnabledChanged(tmrEnabled);
    }

    void setTmrInterval(int tmrInterval)
    {
        mTmrInterval = tmrInterval;
        emit tmrIntervalChanged(tmrInterval);
    }

    void setTmrSampPerInterval(bool sampPerInterval)
    {
        mOneSamplePer = sampPerInterval;
        emit tmrSampPerIntervalChanged(sampPerInterval);
    }

    void setTmrRunning(bool runTimer)
    {
        mTmrRunning = runTimer;
        emit tmrRunningChanged(runTimer);
    }

    void setTmrStopOnStart(bool stopOnStart)
    {
        mStopOnStart = stopOnStart;
        emit tmrStopOnStartChanged(stopOnStart);
    }

    void setTInPrefs(QString tInPrefs)
    {
        mTInPrefs = tInPrefs;
        emit tInPrefsChanged(tInPrefs);
    }

    QString devName() { return mDevName; }
    QString serNum() { return mSerNum; }
    uint8_t devAddress() { return mDevAddress; }
    uint16_t devId() { return mDevID; }
    uint8_t aiRange() { return mRange; }

    bool tmrEnabled() { return mTmrEnabled; }
    int tmrInterval() { return mTmrInterval; }
    bool tmrSampPerInterval() { return mOneSamplePer; }
    bool tmrRunning() { return mTmrRunning; }
    bool tmrStopOnStart() { return mStopOnStart; }
    bool showPlot() { return mShowPlot; }

    u_int32_t scanOptions() { return mScanOptions; }
    int curFunction() { return mCurFunction; }
    TriggerMode triggerType() { return mTriggerType; }
    uint8_t triggerSource() { return mTriggerSource; }
    UtFunctionGroup curFunctionGroup() { return mCurFunctionGroup; }
    QString tInPrefs() { return mTInPrefs; }

    void showQueueConfig() { emit configQueue(); }
    void setUpTimer();
    void saveTInPrefs(QString tInPrefs);

private slots:
    void goTimerRun(bool enable);
    void tmrDialogResponse();

private:
    QWidget *subwidget;
    QString mWindowName;
    QTimer *tmrRunFunc;
    TmrDialog *tmrDialog;

    QString mDevName;
    QString mSerNum;
    uint8_t mDevAddress;
    uint8_t mRange;
    uint16_t mDevID;
    u_int32_t mScanOptions;
    TriggerMode mTriggerType;
    uint8_t mTriggerSource;
    int mAiResolution;

    bool mShowPlot;
    int mCurFunction;

    bool mTmrEnabled = false;
    bool mStopOnStart = false;
    int mTmrInterval;
    bool mOneSamplePer;
    bool mTmrRunning = false;
    QString mTInPrefs;

    UtFunctionGroup mCurFunctionGroup;

    void readWindowPosition();
    void writeWindowPosition();

signals:
    void devNameChanged(QString);
    void serNumChanged(QString);
    void devAddressChanged(uint8_t);
    void devIdChanged(uint16_t);
    void curFunctionChanged(int);
    void showPlotChanged(bool);
    void scanOptionsChanged(u_int32_t);
    void triggerTypeChanged(TriggerMode);
    void triggerSourceChanged(uint8_t);
    void aiRangeChanged(uint8_t);
    void configQueue();

    void tmrEnabledChanged(bool);
    void tmrIntervalChanged(int);
    void tmrRunningChanged(bool);
    void tmrSampPerIntervalChanged(bool);
    void tmrStopOnStartChanged(bool);
    void tInPrefsChanged(QString);
};

#endif // CHILDWINDOW_H
