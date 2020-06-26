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
    Q_PROPERTY(bool plotChecked READ plotChecked WRITE setPlotChecked NOTIFY plotCheckedChanged)

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

    void setPlotChecked(bool trueFalse)
    {
        mPlotIsChecked = trueFalse;
        emit plotCheckedChanged(trueFalse);
    }

    QHash<uint8_t, QString> hatList() { return mHatList; }
    QHash<uint8_t, uint16_t> hatIDList() { return mHatIDList; }
    bool plotChecked() { return mPlotIsChecked; }

    void addFunction(QString funcString);
    void setError(int curError, QString funcText);
    void addDeviceToMenu(QString devName, uint8_t devAddress, uint16_t hatType);
    void removeDeviceFromMenu(uint8_t devAddress);
    void setStatusLabel(QString statusText);
    void savePreferences(QString prefList);

private slots:
    void setBoardMenuSelect(QMdiSubWindow* childWind);
    void createDiscChild();
    void createInfoChild();
    void createAiChild();
    void createAoChild();
    void createDinChild();
    void createDOutChild();
    void createMiscChild();
    void createChild(UtFunctionGroup utFuncGroup, int defaultFunction);
    void changeTrigType();
    void changeSourceType();
    void curFunctionChanged();
    void curOptionChanged();
    void setSelectedDevice();
    void showHistory();
    void showPlot(bool showIt);
    void configureQueue();
    void setTimer();
    void updatePlotMenu(bool enabled);
    void changeRange();
    void changeInputMode(bool modeSet);
    //void setBoardMenuSelect(QMdiSubWindow*);

private:
    Ui::MainWindow *ui;
    QActionGroup *functionGroup;
    QActionGroup *optionGroup;
    QActionGroup *rangeGroup;
    QActionGroup *trigTypeGroup;
    QActionGroup *trigSourceGroup;
    ChildWindow *activeMdiChild() const;
    ErrorDialog errDlg;

    QHash<uint8_t, QString> mHatList;
    QHash<uint8_t, uint16_t> mHatIDList;
    struct HatInfo hatInfoList[MAX_NUMBER_HATS];

    int mCurFunction;
    TriggerMode mTriggerType;
    uint8_t mTriggerSource;
    uint8_t mRange;
    uint32_t mScanOptions;
    QString mOptNames;

    QString mCurBoardName;
    uint8_t mCurAddress;
    uint16_t mCurID;
    QString curFunctionGroupName;

    QStringList mFuncHistoryList;
    int mHistListSize;
    bool mAutoConnect;
    bool mPlotIsChecked;

    void createFuncMenus();
    void readWindowPosition();
    void writeWindowPosition();
    void readHatList();
    uint32_t getSoMask(uint32_t optSelected);

    QHash<uint8_t, uint16_t> m_hatIDList;

signals:
    void hatListChanged(QHash<uint8_t, QString>);
    void hatIDListChanged(QHash<uint8_t, uint16_t>);
    void plotCheckedChanged(bool);
};

#endif // MAINWINDOW_H
