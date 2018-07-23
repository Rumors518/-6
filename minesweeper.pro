#-------------------------------------------------
#
# Project created by QtCreator 2018-07-19T16:02:08
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = minesweeper
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp\
        mainwindow.cpp \
    item.cpp

HEADERS  += mainwindow.h \
    item.h

FORMS    += mainwindow.ui

RESOURCES += \
    rc.qrc

RC_ICONS=myico.ico

CONFIG+=resources_big

QT += multimedia
