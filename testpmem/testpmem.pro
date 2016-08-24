TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp


LIBS += -L$$PWD/../build-purempool-Desktop_Qt_5_3_MSVC2013_64bit-Release/release/ -lpurempool

INCLUDEPATH += $$PWD/../purempool
DEPENDPATH += $$PWD/../purempool

PRE_TARGETDEPS += $$PWD/../build-purempool-Desktop_Qt_5_3_MSVC2013_64bit-Release/release/purempool.lib

#LIBS += -L$$PWD/../build-purempool-Desktop_Qt_5_3_MSVC2013_64bit-Debug/debug/ -lpurempool

#INCLUDEPATH += $$PWD/../purempool
#DEPENDPATH += $$PWD/../purempool

#PRE_TARGETDEPS += $$PWD/../build-purempool-Desktop_Qt_5_3_MSVC2013_64bit-Debug/debug/purempool.lib


CONFIG+=c++11
