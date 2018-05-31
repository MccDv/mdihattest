#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "errordialog.h"
#include <QMainWindow>
#include <QActionGroup>
#include <QMap>
//#include "hatdevice.h"
#include "unitest.h"
#include "childwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void closeEvent(QCloseEvent *event);

    void addFunction(QString funcString);
    void setError(int curError, QString funcText);
    void addDeviceToMenu(QString devName, uint8_t devAddress);
    void removeDeviceFromMenu(uint8_t devAddress);

private slots:
    void setBoardMenuSelect(QMdiSubWindow* childWind);
    void createDiscChild();
    void createInfoChild();
    void createAiChild();
    void createChild(UtFunctionGroup utFuncGroup, int defaultFunction);
    void changeTrigType();
    void curFunctionChanged();
    void curOptionChanged();
    void setSelectedDevice();
    void showHistory();
    void showPlot(bool showIt);

private:
    Ui::MainWindow *ui;
    QActionGroup *functionGroup;
    QActionGroup *optionGroup;
    QActionGroup *trigTypeGroup;
    ChildWindow *activeMdiChild() const;
    ErrorDialog errDlg;

    QHash<int, QString> hatList;

    int mCurFunction;
    TriggerMode mTriggerType;
    uint32_t mScanOptions;
    QString mOptNames;

    QString mCurBoardName;
    uint8_t mCurAddress;
    QString curFunctionGroupName;

    QStringList mFuncHistoryList;
    int mHistListSize;

    void readWindowPosition();
    void writeWindowPosition();
    uint32_t getSoMask(uint32_t optSelected);
};

#endif // MAINWINDOW_H
