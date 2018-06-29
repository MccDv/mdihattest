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
    Q_PROPERTY(uint8_t devAddress READ devAddress WRITE setDevAddress NOTIFY devAddressChanged)
    Q_PROPERTY(uint16_t devId READ devId WRITE setDevId NOTIFY devIdChanged)
    Q_PROPERTY(bool showPlot READ showPlot WRITE setShowPlot NOTIFY showPlotChanged)
    Q_PROPERTY(u_int32_t scanOptions READ scanOptions WRITE setScanOptions NOTIFY scanOptionsChanged)
    Q_PROPERTY(int curFunction READ curFunction WRITE setCurFunction NOTIFY curFunctionChanged)
    Q_PROPERTY(TriggerMode triggerType READ triggerType WRITE setTriggerType NOTIFY triggerTypeChanged)

    Q_PROPERTY(bool tmrEnabled READ tmrEnabled WRITE setTmrEnabled NOTIFY tmrEnabledChanged)
    Q_PROPERTY(int tmrInterval READ tmrInterval WRITE setTmrInterval NOTIFY tmrIntervalChanged)
    Q_PROPERTY(bool tmrStopOnStart READ tmrStopOnStart WRITE setTmrStopOnStart NOTIFY tmrStopOnStartChanged)
    Q_PROPERTY(bool tmrRunning READ tmrRunning WRITE setTmrRunning NOTIFY tmrRunningChanged)

public:
    explicit ChildWindow(QWidget *parent = 0, UtFunctionGroup funcGroup = FUNC_GROUP_DISC);
    ~ChildWindow();
    void closeEvent(QCloseEvent *event);

    void setDevName(QString devName)
    {
        mDevName = devName;
        emit devNameChanged(devName);
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

    QString devName() { return mDevName; }
    uint8_t devAddress() { return mDevAddress; }
    uint16_t devId() { return mDevID; }

    bool tmrEnabled() { return mTmrEnabled; }
    int tmrInterval() { return mTmrInterval; }
    bool tmrRunning() { return mTmrRunning; }
    bool tmrStopOnStart() { return mStopOnStart; }
    bool showPlot() { return mShowPlot; }

    u_int32_t scanOptions() { return mScanOptions; }
    int curFunction() { return mCurFunction; }
    TriggerMode triggerType() { return mTriggerType; }
    UtFunctionGroup curFunctionGroup() { return mCurFunctionGroup; }
    void showQueueConfig() { emit configQueue(); }
    void setUpTimer();

private slots:
    void goTimerRun(bool enable);
    void tmrDialogResponse();

private:
    QWidget *subwidget;
    QString windowName;
    QTimer *tmrRunFunc;
    TmrDialog *tmrDialog;

    QString mDevName;
    uint8_t mDevAddress;
    uint16_t mDevID;
    u_int32_t mScanOptions;
    TriggerMode mTriggerType;
    int mAiResolution;

    bool mShowPlot;
    int mCurFunction;

    bool mTmrEnabled = false;
    bool mStopOnStart = false;
    int mTmrInterval;
    bool mTmrRunning = false;

    UtFunctionGroup mCurFunctionGroup;

    void readWindowPosition();
    void writeWindowPosition();

signals:
    void devNameChanged(QString);
    void devAddressChanged(uint8_t);
    void devIdChanged(uint16_t);
    void curFunctionChanged(int);
    void showPlotChanged(bool);
    void scanOptionsChanged(u_int32_t);
    void triggerTypeChanged(TriggerMode);
    void configQueue();

    void tmrEnabledChanged(bool);
    void tmrIntervalChanged(int);
    void tmrRunningChanged(bool);
    void tmrStopOnStartChanged(bool);
};

#endif // CHILDWINDOW_H
