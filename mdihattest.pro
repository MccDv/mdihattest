#-------------------------------------------------
#
# Project created by QtCreator 2018-05-12T10:35:29
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = orghattest
TEMPLATE = app

CONFIG += console

# Suppress QMap parameter error for newer qt library
QMAKE_CXXFLAGS += -Wno-psabi

# QMAKE_CXXFLAGS_DEBUG -= -O
# QMAKE_CXXFLAGS_DEBUG -= -O1
# QMAKE_CXXFLAGS_DEBUG -= -O2
# QMAKE_CXXFLAGS_DEBUG += -O0

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
# DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# MCC 128 development: HAT_06
# MCC 172 development: HAT_05
# MCC 152 development: HAT_04
# MCC 134 development: HAT_03

DEFINES += HAT_03
DEFINES += HAT_04
DEFINES += HAT_05
DEFINES += HAT_06

SOURCES += \
    main.cpp \
    qcustomplot.cpp \
    testutilites.cpp \
    errordialog.cpp \
    mainwindow.cpp \
    childwindow.cpp \
    hatdevice.cpp \
    hatdiscovery.cpp \
    infoform.cpp \
    queuedialog.cpp \
    hatinterface.cpp \
    tmrdialog.cpp \
    aout.cpp \
    dioform.cpp \
    miscform.cpp

HEADERS += \
    qcustomplot.h \
    unitest.h \
    testutilities.h \
    errordialog.h \
    mainwindow.h \
    childwindow.h \
    hatdevice.h \
    hatdiscovery.h \
    infoform.h \
    queuedialog.h \
    hatinterface.h \
    tmrdialog.h \
    aout.h \
    dioform.h \
    miscform.h

FORMS += \
    errordialog.ui \
    mainwindow.ui \
    hatdevice.ui \
    hatdiscovery.ui \
    infoform.ui \
    queuedialog.ui \
    tmrdialog.ui \
    aout.ui \
    dioform.ui \
    miscform.ui

target.path = /home/pi
INSTALLS += target

unix:!macx: LIBS += -L$$PWD/../../../../opt/qtrpi/raspbian/sysroot/usr/lib/ -ldaqhats

INCLUDEPATH += $$PWD/../../../../opt/qtrpi/raspbian/sysroot/usr/include
DEPENDPATH += $$PWD/../../../../opt/qtrpi/raspbian/sysroot/usr/include
