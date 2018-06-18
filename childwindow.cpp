#include <QtGui>
#include <QWidget>
#include <QMdiSubWindow>
#include "childwindow.h"
#include "hatdevice.h"
#include "hatdiscovery.h"
#include "infoform.h"
#include "mainwindow.h"
#include "hatinterface.h"

MainWindow* getMainWindow();
//MainWindow *mMainWindow;

ChildWindow::ChildWindow(QWidget *parent, UtFunctionGroup funcGroup) : QMdiSubWindow(parent)
{
    switch (funcGroup) {
    case FUNC_GROUP_AIN:
        subwidget = new HatDevice(this);
        windowName = "ainwindow";
        mCurFunction = UL_AIN;
        break;
    case FUNC_GROUP_MISC:
        subwidget = new InfoForm(this);
        windowName = "infowindow";
        mCurFunction = UL_AI_INFO;
        break;
    default:
        subwidget = new HatDiscovery(this);
        windowName = "dscvrwindow";
        mCurFunction = UL_DISC;
        break;
    }
    mScanOptions = 0;
    mTriggerType = TRIG_RISING_EDGE;
    mAiResolution = 12;
    mCurFunctionGroup = funcGroup;

    //MainWindow *mainParent = qobject_cast<MainWindow *>(parent);
    this->setWidget(subwidget);
    connect(this, SIGNAL(devNameChanged(QString)), subwidget, SLOT(updateParameters()));
    connect(this, SIGNAL(devAddressChanged(uint8_t)), subwidget, SLOT(updateParameters()));
    connect(this, SIGNAL(devIdChanged(uint16_t)), subwidget, SLOT(updateParameters()));
    connect(this, SIGNAL(curFunctionChanged(int)), subwidget, SLOT(functionChanged(int)));
    connect(this, SIGNAL(showPlotChanged(bool)), subwidget, SLOT(showPlotWindow(bool)));
    connect(this, SIGNAL(scanOptionsChanged(u_int32_t)), subwidget, SLOT(updateParameters()));
    connect(this, SIGNAL(triggerTypeChanged(TriggerMode)), subwidget, SLOT(updateParameters()));
    connect(this, SIGNAL(configQueue()), subwidget, SLOT(showQueueConfig()));

    readWindowPosition();
}

void ChildWindow::closeEvent(QCloseEvent *event)
{
    //int response;
    HatInterface *hatInterface;
    hatInterface = new HatInterface;

    if(hatInterface->deviceIsOpen(mDevID, mDevAddress)) {
        if(mDevID == HAT_ID_MCC_118)
            hatInterface->aInScanCleanup(mDevID, mDevAddress);
        hatInterface->closeDevice(mDevID, mDevAddress);
    }
    writeWindowPosition();
    event->accept();
    this->deleteLater();
}

ChildWindow::~ChildWindow()
{
    subwidget->~QWidget();
}

MainWindow* getMainWindow()
{
    foreach (QWidget *w, QApplication::topLevelWidgets())
        if (QMainWindow* mainWin = qobject_cast<QMainWindow*>(w))
            return qobject_cast<MainWindow *>(mainWin);
    return nullptr;
}

void ChildWindow::readWindowPosition()
{
    QSettings windowSettings("Measurement Computing", "Qt Hat Test Linux");

    windowSettings.beginGroup(windowName);

    restoreGeometry(windowSettings.value("geometry", saveGeometry()).toByteArray());
    //restoreState(windowSettings.value("savestate", saveState()).toByteArray());
    move(windowSettings.value("pos", pos()).toPoint());
    resize(windowSettings.value("size", size()).toSize());
    if (windowSettings.value("maximized", isMaximized()).toBool())
        showMaximized();

    windowSettings.endGroup();
}

void ChildWindow::writeWindowPosition()
{
    QSettings windowSettings("Measurement Computing", "Qt Hat Test Linux");

    windowSettings.beginGroup(windowName);

    windowSettings.setValue("geometry", saveGeometry());
    //windowSettings.setValue("savestate", saveState());
    windowSettings.setValue("maximized", isMaximized());
    if (!isMaximized()) {
        windowSettings.setValue("pos", pos());
        windowSettings.setValue("size", size());
    }
    windowSettings.endGroup();
}
