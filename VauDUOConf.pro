#-------------------------------------------------
#
# Project created by QtCreator 2015-05-25T23:26:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport


TARGET = VauDUOConf
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    serialthread.cpp

HEADERS  += mainwindow.h \
    serialthread.h \
    telemetry.h

FORMS    += mainwindow.ui
