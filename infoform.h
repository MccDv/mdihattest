#ifndef INFOFORM_H
#define INFOFORM_H

#include <QWidget>
#include <QFile>
#include <QTime>
#include <QTextStream>
#include "hatinterface.h"
#include "mcchats/mcchats.h"

namespace Ui {
class InfoForm;
}

class InfoForm : public QWidget
{
    Q_OBJECT

public:
    explicit InfoForm(QWidget *parent = 0);
    ~InfoForm();

private slots:
    void showSysInfo();
    void findHats();
    void devSelectedChanged(QString);
    void functionChanged(int utFunction);
    void runSelectedFunction();
    void showBoardParameters();
    void showPlotWindow(bool showIt);
    void updateParameters();
    void showQueueConfig();
    void readCal();
    void writeCal();
    void flashLED();

private:
    Ui::InfoForm *ui;
    HatInterface *hatInterface;
    QHash<uint8_t, QString> mHatList;
    QHash<uint8_t, uint16_t> mHatIDList;

    int mNumHats;
    QString mDevName;
    int mResponse;
    uint8_t mAddress;
    uint16_t mHatID;
};

#endif // INFOFORM_H
