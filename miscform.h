#ifndef MISCFORM_H
#define MISCFORM_H

#include <QWidget>
#include <QMessageBox>
#include <QThread>
#include "unitest.h"
#include "errordialog.h"
#include "testutilities.h"
#include "hatinterface.h"

namespace Ui {
class MiscForm;
}

class MiscForm : public QWidget
{
    Q_OBJECT

public:
    explicit MiscForm(QWidget *parent = 0);
    ~MiscForm();
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    static void eventCallback(void *userData);

private slots:
    void updateParameters();
    void functionChanged(int utFunction);
    void groupChanged(int newGroup);
    void onClickCmdGo();
    void onClickCmdStop();
    void runSelectedFunction();
    void toggleGoTimer(bool enableTimer);
    void showPlotWindow(bool showIt);
    void showQueueConfig();
    void onCallback();
    void runEventEnable();
    void runEventDisable();

private:
    Ui::MiscForm *ui;
    HatInterface *hatInterface;
    QTimer *tmrGoTimer;
    ErrorDialog errDlg;
    QHash<uint8_t, QString> mHatList;
    QHash<uint8_t, uint16_t> mHatIDList;

    typedef void (*callbackFunction)(void *userData);

    int mNumHats;
    int mDevIndex;
    int mResponse;
    uint8_t mAddress;
    uint16_t mHatID;
    QString mDevName;

    QString mFuncName;
    int mUtFunction;
    int mCurGroup;

    bool mUseTimer;
    bool mGoTmrIsRunning;
    bool mStopOnStart;
    bool mOneSampPerForTotalSamps;
    int mTmrInterval;

    int mNumBits;
    int mDioResolution;
    int mHistListSize;

    void initDevices();
    void waitForInterupt();
    void getInterruptStatus();

signals:
    void callbackTriggered();
};

#endif // MISCFORM_H
