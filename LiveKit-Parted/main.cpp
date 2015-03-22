#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //w.show();
    parted_widget *widget = new parted_widget;
    widget->show();

    return a.exec();
}
