#-------------------------------------------------
#
# Project created by QtCreator 2014-08-08T00:12:28
#
#-------------------------------------------------

QT       -= core gui

TARGET = purempool
TEMPLATE = lib
CONFIG += staticlib

SOURCES += purempool.cpp

HEADERS += purempool.h \
    Common.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
