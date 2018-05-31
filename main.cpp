#include "mainwindow.h"
#include <QApplication>
#include <QLoggingCategory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    /*QLoggingCategory::setFilterRules("*.debug=true\n"
                                     "qt.qpa.input*.debug=false");
    qDebug() << "Debugging";*/
    MainWindow w;
    w.show();

    return a.exec();
}
