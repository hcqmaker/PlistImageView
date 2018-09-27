#-------------------------------------------------
#
# Project created by QtCreator 2018-09-26T16:58:27
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PlistImageView
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    frameinfo.cpp

HEADERS  += mainwindow.h \
    frameinfo.h

FORMS    += mainwindow.ui

RESOURCES += \
    plistimageview.qrc
