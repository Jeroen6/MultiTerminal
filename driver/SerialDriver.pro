#-------------------------------------------------
#
# Project created by QtCreator 2014-02-17T19:56:16
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = SerialDriver
CONFIG   += console
CONFIG   -= app_bundle
CONFIG += file_copies

TEMPLATE = app


SOURCES += main.cpp \
    serialdriver.cpp

QT += serialport
QT += network

HEADERS += \
    serialdriver.h

OTHER_FILES += \
    BUGS.txt \
    TODO.txt

