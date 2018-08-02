#ifndef DIOFORM_H
#define DIOFORM_H

#include <QWidget>
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

private slots:
    void initDeviceParams();
    void updateParameters();

    void functionChanged(int utFunction);
    void groupChanged(int newGroup);
    void bitToggled(int bitNumber);
    void onClickCmdGo();
    void onClickCmdStop();
    void configureInputs();
    void configureOutputs();
    void runSelectedFunction();
    void stopGoTimer();
    void showPlotWindow(bool showIt);
    void showQueueConfig();

private:
    Ui::DioForm *ui;
    HatInterface *hatInterface;
    ErrorDialog errDlg;
    int numBitCheckboxes = 8;
    QCheckBox *chkBit[8]={};

    QString mFuncName;
    int mUtFunction;
    int mCurGroup;
    bool mUseTimer;

    int mNumHats;
    int mDevIndex;
    int mResponse;
    uint8_t mAddress;
    uint16_t mHatID;
    QString mDevName;

    bool mPlot;

    int mDioResolution;

    void setUiForGroup();
    void setUiForFunction();
    void createBitBoxes();
    void runDInFunc();
    void runDBitInFunc(uint8_t value, uint8_t bit);
    void runDBitOutFunc(uint8_t value, uint8_t bit);
    void runDOutFunc();
    //void runDBitOutFunc(DigitalPortType portType, int bitNum, unsigned int bitValue);
};

#endif // DIOFORM_H
