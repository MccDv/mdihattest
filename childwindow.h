#ifndef CHILDWINDOW_H
#define CHILDWINDOW_H

#include <QtGui>
#include <QWidget>
#include <QMdiSubWindow>
#include "unitest.h"
#include "mcchats/mcchats.h"
//#include "mainwindow.h"
//#include "hatdevice.h"
//#include "hatdiscovery.h"

namespace Ui {
    class ChildWindow;
}

class ChildWindow : public QMdiSubWindow
{
    Q_OBJECT
    Q_PROPERTY(QString devName READ devName WRITE setDevName NOTIFY devNameChanged)
    Q_PROPERTY(uint8_t devAddress READ devAddress WRITE setDevAddress NOTIFY devAddressChanged)
    Q_PROPERTY(bool showPlot READ showPlot WRITE setShowPlot NOTIFY showPlotChanged)
    Q_PROPERTY(u_int32_t scanOptions READ scanOptions WRITE setScanOptions NOTIFY scanOptionsChanged)
    Q_PROPERTY(int curFunction READ curFunction WRITE setCurFunction NOTIFY curFunctionChanged)
    Q_PROPERTY(TriggerMode triggerType READ triggerType WRITE setTriggerType NOTIFY triggerTypeChanged)

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

    QString devName() { return mDevName; }
    uint8_t devAddress() { return mDevAddress; }
    bool showPlot() { return mShowPlot; }
    u_int32_t scanOptions() { return mScanOptions; }
    int curFunction() { return mCurFunction; }
    TriggerMode triggerType() { return mTriggerType; }
    UtFunctionGroup curFunctionGroup() { return mCurFunctionGroup; }

private:
    QWidget *subwidget;
    QString windowName;

    QString mDevName;
    uint8_t mDevAddress;
    u_int32_t mScanOptions;
    TriggerMode mTriggerType;
    int mAiResolution;

    bool mShowPlot;
    int mCurFunction;
    UtFunctionGroup mCurFunctionGroup;

    void readWindowPosition();
    void writeWindowPosition();

signals:
    void devNameChanged(QString);
    void devAddressChanged(uint8_t);
    void curFunctionChanged(int);
    void showPlotChanged(bool);
    void scanOptionsChanged(u_int32_t);
    void triggerTypeChanged(TriggerMode);

};

#endif // CHILDWINDOW_H
