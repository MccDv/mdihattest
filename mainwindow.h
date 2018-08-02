#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "errordialog.h"
#include <QMainWindow>
#include <QActionGroup>
#include <QMap>
#include "unitest.h"
#include "childwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(QHash<uint8_t, QString> hatList READ hatList WRITE setHatList NOTIFY hatListChanged)
    Q_PROPERTY(QHash<uint8_t, uint16_t> hatIDList READ hatIDList WRITE setHatIDList NOTIFY hatIDListChanged)

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void closeEvent(QCloseEvent *event);

    void setHatList(QHash<uint8_t, QString> hatList)
    {
        mHatList = hatList;
        emit hatListChanged(hatList);
    }

    void setHatIDList(QHash<uint8_t, uint16_t> hatIDList)
    {
        mHatIDList = hatIDList;
        emit hatIDListChanged(hatIDList);
    }

    QHash<uint8_t, QString> hatList() { return mHatList; }
    QHash<uint8_t, uint16_t> hatIDList() { return mHatIDList; }

    void addFunction(QString funcString);
    void setError(int curError, QString funcText);
    void addDeviceToMenu(QString devName, uint8_t devAddress, uint16_t hatType);
    void removeDeviceFromMenu(uint8_t devAddress);
    void setStatusLabel(QString statusText);

private slots:
    void setBoardMenuSelect(QMdiSubWindow* childWind);
    void createDiscChild();
    void createInfoChild();
    void createAiChild();
    void createAoChild();
    void createDinChild();
    void createDOutChild();
    void createChild(UtFunctionGroup utFuncGroup, int defaultFunction);
    void changeTrigType();
    void curFunctionChanged();
    void curOptionChanged();
    void setSelectedDevice();
    void showHistory();
    void showPlot(bool showIt);
    void configureQueue();
    void setTimer();

private:
    Ui::MainWindow *ui;
    QActionGroup *functionGroup;
    QActionGroup *optionGroup;
    QActionGroup *trigTypeGroup;
    ChildWindow *activeMdiChild() const;
    ErrorDialog errDlg;

    QHash<uint8_t, QString> mHatList;
    QHash<uint8_t, uint16_t> mHatIDList;

    int mCurFunction;
    TriggerMode mTriggerType;
    uint32_t mScanOptions;
    QString mOptNames;

    QString mCurBoardName;
    uint8_t mCurAddress;
    uint16_t mCurID;
    QString curFunctionGroupName;

    QStringList mFuncHistoryList;
    int mHistListSize;

    void createFuncMenus();
    void readWindowPosition();
    void writeWindowPosition();
    uint32_t getSoMask(uint32_t optSelected);

signals:
    void hatListChanged(QHash<uint8_t, QString>);
    void hatIDListChanged(QHash<uint8_t, uint16_t>);
};

#endif // MAINWINDOW_H
