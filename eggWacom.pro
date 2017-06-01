#-------------------------------------------------
#
# Project created by QtCreator 2015-05-29T22:02:09
#
#-------------------------------------------------

QT       += core gui\
            serialport\
            svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = eggWacom
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    wacomapplication.cpp \
    wacomcanvas.cpp \
    myshape.cpp \
    gcodeset.cpp \
    gcodesetdata.cpp \
    widthsetdialog.cpp \
    gcode.cpp \
    spherebot.cpp \
    spherebotport.cpp \
    transceiverthread.cpp \
    newdialog.cpp

HEADERS  += mainwindow.h \
    wacomapplication.h \
    wacomcanvas.h \
    myshape.h \
    datadef.h \
    gcodeset.h \
    gcodesetdata.h \
    widthsetdialog.h \
    gcode.h \
    spherebot.h \
    spherebotport.h \
    transceiverthread.h \
    newdialog.h

FORMS    += mainwindow.ui \
    spherebot.ui

CONFIG += c++11
