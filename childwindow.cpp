#include <QtGui>
#include <QWidget>
#include <QMdiSubWindow>
#include "childwindow.h"
#include "hatdevice.h"
#include "aout.h"
#include "dioform.h"
#include "hatdiscovery.h"
#include "infoform.h"
#include "miscform.h"
#include "mainwindow.h"
#include "hatinterface.h"

MainWindow* getMainWindow();
//MainWindow *mMainWindow;

ChildWindow::ChildWindow(QWidget *parent, UtFunctionGroup funcGroup) : QMdiSubWindow(parent)
{
    switch (funcGroup) {
    case FUNC_GROUP_AIN:
        subwidget = new HatDevice(this);
        mWindowName = "ainwindow";
        mCurFunction = UL_AIN;
        break;
    case FUNC_GROUP_AOUT:
        subwidget = new AOut(this);
        mWindowName = "aoutwindow";
        mCurFunction = UL_AOUT;
        break;
    case FUNC_GROUP_DIN:
        subwidget = new DioForm(this);
        mWindowName = "dinwindow";
        break;
    case FUNC_GROUP_DOUT:
        subwidget = new DioForm(this);
        mWindowName = "doutwindow";
        break;
    case FUNC_GROUP_MISC:
        subwidget = new InfoForm(this);
        mWindowName = "infowindow";
        mCurFunction = UL_AI_INFO;
        break;
    case FUNC_GROUP_STATUS:
        subwidget = new MiscForm(this);
        mWindowName = "miscform";
        mCurFunction = UL_GET_STATUS;
        break;
    default:
        subwidget = new HatDiscovery(this);
        mWindowName = "dscvrwindow";
        mCurFunction = UL_DISC;
        break;
    }

    mTriggerType = TRIG_RISING_EDGE;
    mTriggerSource = SOURCE_LOCAL;
    mAiResolution = 12;
    mRange = A_IN_RANGE_BIP_10V;
    mCurFunctionGroup = funcGroup;
    tmrRunFunc = new QTimer(this);
    mTmrEnabled = false;
    mTmrInterval = 0;
    mOneSamplePer = false;

    //MainWindow *mainParent = qobject_cast<MainWindow *>(parent);
    this->setWidget(subwidget);
    connect(this, SIGNAL(devNameChanged(QString)), subwidget, SLOT(updateParameters()));
    connect(this, SIGNAL(serNumChanged(QString)), subwidget, SLOT(updateParameters()));
    connect(this, SIGNAL(devAddressChanged(uint8_t)), subwidget, SLOT(updateParameters()));
    connect(this, SIGNAL(devIdChanged(uint16_t)), subwidget, SLOT(updateParameters()));
    connect(this, SIGNAL(curFunctionChanged(int)), subwidget, SLOT(functionChanged(int)));
    connect(this, SIGNAL(showPlotChanged(bool)), subwidget, SLOT(showPlotWindow(bool)));
    connect(this, SIGNAL(scanOptionsChanged(u_int32_t)), subwidget, SLOT(updateParameters()));
    connect(this, SIGNAL(triggerTypeChanged(TriggerMode)), subwidget, SLOT(updateParameters()));
    connect(this, SIGNAL(aiRangeChanged(uint8_t)), subwidget, SLOT(updateParameters()));
    connect(this, SIGNAL(triggerSourceChanged(uint8_t)), subwidget, SLOT(updateParameters()));
    connect(this, SIGNAL(configQueue()), subwidget, SLOT(showQueueConfig()));
    connect(tmrRunFunc, SIGNAL(timeout()), subwidget, SLOT(runSelectedFunction()));
    connect(this, SIGNAL(tmrRunningChanged(bool)), this, SLOT(goTimerRun(bool)));
    connect(this, SIGNAL(tmrIntervalChanged(int)), subwidget, SLOT(updateParameters()));

    readWindowPosition();
    if (funcGroup == FUNC_GROUP_AIN)
        mScanOptions = OPTS_IFC_BACKGROUND;
    else
        mScanOptions = OPTS_DEFAULT;
}

void ChildWindow::closeEvent(QCloseEvent *event)
{
    writeWindowPosition();
    event->accept();
    this->deleteLater();
}

ChildWindow::~ChildWindow()
{
    subwidget->~QWidget();
}

void ChildWindow::goTimerRun(bool enable)
{
    if (enable) {
        tmrRunFunc->start(mTmrInterval);
    } else {
        tmrRunFunc->stop();
    }
    emit tmrEnabledChanged(true);
}

MainWindow* getMainWindow()
{
    foreach (QWidget *w, QApplication::topLevelWidgets())
        if (QMainWindow* mainWin = qobject_cast<QMainWindow*>(w))
            return qobject_cast<MainWindow *>(mainWin);
    return nullptr;
}

void ChildWindow::setUpTimer()
{
    tmrDialog = new TmrDialog(this);
    connect(tmrDialog, SIGNAL(accepted()), this, SLOT(tmrDialogResponse()));
    if (mTmrInterval == 0)
        mTmrInterval = 1000;
    tmrDialog->setEnabled(mTmrEnabled);
    tmrDialog->setInterval(mTmrInterval);
    tmrDialog->setOnePerInterval(mOneSamplePer);
    tmrDialog->setStopOnStart(mStopOnStart);
    tmrDialog->exec();
}

void ChildWindow::tmrDialogResponse()
{
    mTmrEnabled = tmrDialog->enabled();
    mTmrInterval = tmrDialog->interval();
    mStopOnStart = tmrDialog->stopOnStart();
    mOneSamplePer = tmrDialog->onePerInterval();
    disconnect(tmrDialog);
    delete tmrDialog;
    emit tmrIntervalChanged(mTmrInterval);
}

void ChildWindow::readWindowPosition()
{
    QSettings windowSettings("Measurement Computing", "Qt Hat Test Linux");

    windowSettings.beginGroup(mWindowName);
    restoreGeometry(windowSettings.value("geometry", saveGeometry()).toByteArray());
    move(windowSettings.value("pos", pos()).toPoint());
    resize(windowSettings.value("size", size()).toSize());
    if (windowSettings.value("maximized", isMaximized()).toBool())
        showMaximized();
    windowSettings.endGroup();

}

void ChildWindow::writeWindowPosition()
{
    QSettings windowSettings("Measurement Computing", "Qt Hat Test Linux");

    windowSettings.beginGroup(mWindowName);

    windowSettings.setValue("geometry", saveGeometry());
    windowSettings.setValue("maximized", isMaximized());
    if (!isMaximized()) {
        windowSettings.setValue("pos", pos());
        windowSettings.setValue("size", size());
    }
    windowSettings.endGroup();

    //mTInPrefs = "3,4,255,255";
    if(mWindowName == "ainwindow") {
        windowSettings.beginGroup(mSerNum);
        windowSettings.setValue("TempPrefs", mTInPrefs);
        windowSettings.endGroup();
    }

}
