#-------------------------------------------------
#
# Project created by QtCreator 2020-07-20T09:13:23
#
#-------------------------------------------------


QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = simu
TEMPLATE = app

CONFIG += c++11
# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += AFS_FRAMEWORK

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    modulesmanager.cpp \
    udpsocket.cpp

HEADERS += \
        mainwindow.h \
    modulesmanager.h \
    declare.h \
    frameinterface.h \
    modulesinterface.h \
    udpsocket.h \
    ../Interface/csysctrl.h \
    csysctrl.h

FORMS += \
        mainwindow.ui \
    modulesmanager.ui

RESOURCES += \
    image/res.qrc

DISTFILES +=

