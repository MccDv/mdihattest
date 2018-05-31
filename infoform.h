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
    void showBoardParameters();
    void readCal();

private:
    Ui::InfoForm *ui;
    struct HatInfo hatInfoList[MAX_NUMBER_HATS];

    int mNumHats;
    int mDevIndex;
    int mResponse;
    uint8_t mAddress;
};

#endif // INFOFORM_H
