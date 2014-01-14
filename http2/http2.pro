#-------------------------------------------------
#
# Project created by QtCreator 2011-03-10T14:50:03
#
#-------------------------------------------------

QT      += core

QT      += gui
QT      += network
QT += widgets

TARGET = http2
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
    ../MySocketServer.cpp \
    ../MySocketClient.cpp \
    ../main.cpp \
    ../dialog.cpp \
    ../statistiques.cpp

HEADERS += \
    ../MySocketServer.h \
    ../MySocketClient.h \
    ../dialog.h \
    ../statistiques.h
