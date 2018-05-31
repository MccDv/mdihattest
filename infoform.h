#ifndef INFOFORM_H
#define INFOFORM_H

#include <QWidget>
#include <QFile>
#include <QTime>
#include <QTextStream>
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
    void readCal();
    void flashLED();

private:
    Ui::InfoForm *ui;
    QHash<uint8_t, QString> mHatList;

    int mNumHats;
    QString mDevName;
    int mResponse;
    uint8_t mAddress;
};

#endif // INFOFORM_H
