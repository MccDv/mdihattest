#ifndef DIOFORM_H
#define DIOFORM_H

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
class DioForm;
}

class DioForm : public QWidget
{
    Q_OBJECT

public:
    explicit DioForm(QWidget *parent = 0);
    ~DioForm();
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);

private slots:
    void initDeviceParams();
    void updateParameters();

    void functionChanged(int utFunction);
    void groupChanged(int newGroup);
    void bitToggled(int bitNumber);
    void onClickCmdGo();
    void onClickCmdStop();
    void readOutputs();
    void writeConfiguration();
    void runSelectedFunction();
    void toggleGoTimer(bool enableTimer);
    void showPlotWindow(bool showIt);
    void showQueueConfig();
    void readConfig();
    void configItemChanged();
    void runDioReset();

private:
    Ui::DioForm *ui;
    HatInterface *hatInterface;
    QTimer *tmrGoTimer;
    ErrorDialog errDlg;
    int numBitCheckboxes = 8;
    QCheckBox *chkBit[8]={};

    QString mFuncName;
    QString mGroupName;
    int mUtFunction;
    int mCurGroup;
    bool mFirstCall;

    int mNumHats;
    int mDevIndex;
    int mResponse;
    uint8_t mAddress;
    uint16_t mHatID;
    QString mDevName;

    bool mUseTimer;
    bool mGoTmrIsRunning;
    bool mStopOnStart;
    bool mOneSampPerForTotalSamps;
    int mTmrInterval;

    bool mPlot;
    uint8_t mConfigItem;

    int mNumBits;
    int mDioResolution;
    int mHistListSize;

    void setUiForGroup();
    void setUiForFunction();
    void createBitBoxes();
    void readPort();
    void readBits();
    void runDInFunc();
    void runIntPortFunc();
    void runDBitInFunc();
    void runIntBitFunc();
    void waitForInterupt();
    void writePort();
    void writeBit();
    void runDOutFunc();
    void runDBitOutFunc(uint8_t bit, uint8_t value);
    void runDBitConfigFunc(uint8_t bit, uint8_t value);
    void disableCheckboxInput(bool disableMouse);
    void setDefaultBits(uint8_t portType);
    //void runDBitOutFunc(DigitalPortType portType, int bitNum, unsigned int bitValue);
};

#endif // DIOFORM_H
