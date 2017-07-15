#-------------------------------------------------
#
# Project created by QtCreator 2017-07-15T11:53:26
#
#-------------------------------------------------

QT+= core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

isEqual(QT_MAJOR_VERSION, 5) {
        cache()
        DEFINES +=QT5BUILD
}

TARGET = Resubmission
TEMPLATE = app

OBJECTS_DIR=obj
MOC_DIR=moc
CONFIG -= app_bundle

SOURCES+=$$PWD/src/main.cpp\
         $$PWD/src/mainwindow.cpp \
         $$PWD/src/fluidsimulator.cpp \
         $$PWD/src/viewer.cpp

HEADERS+=$$PWD/include/mainwindow.h \
         $$PWD/include/fluidsimulator.h \
         $$PWD/include/viewer.h

INCLUDEPATH+=./include

FORMS    += mainwindow.ui
