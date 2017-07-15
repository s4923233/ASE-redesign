#-------------------------------------------------
#
# Project created by QtCreator 2017-07-15T11:53:26
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Resubmission
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    fluidsimulator.cpp \
    viewer.cpp

HEADERS  += mainwindow.h \
    fluidsimulator.h \
    viewer.h

FORMS    += mainwindow.ui
