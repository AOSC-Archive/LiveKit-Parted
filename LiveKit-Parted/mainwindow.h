#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "parted_widget.h"



class MainWindow : public QMainWindow{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
private:
};

#endif // MAINWINDOW_H
