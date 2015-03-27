#-------------------------------------------------
#
# Project created by QtCreator 2015-03-22T12:04:10
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LiveKit-Parted
TEMPLATE = app


SOURCES +=  main.cpp            \
            mainwindow.cpp      \
            parted_widget.cpp

HEADERS  += mainwindow.h        \
            parted_widget.h

LIBS += -lparted
