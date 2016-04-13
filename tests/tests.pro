#-------------------------------------------------
#
# Project created by QtCreator 2016-03-31T09:32:57
#
#-------------------------------------------------
TEMPLATE = app

QT += core testlib

TARGET = tests
CONFIG += testcase
CONFIG -= debug_and_release
CONFIG += console
#CONFIG   -= app_bundle

QMAKE_CXXFLAGS += -O3

# Default rules for deployment.
include(../deployment.pri)

include(../defaults.pri)
include(../3rdparty/GoogleTest/GoogleTest.pri)

INCLUDEPATH += ../nutmeglib/src
LIBS += -L../nutmeglib -lnutmeglib

#INCLUDEPATH += /Users/kitham/projects/packages/armadillo-6.700.3/include
#LIBS += -L/Users/kitham/projects/packages/armadillo-6.700.3 -larmadillo

SOURCES += main.cpp \
    testndarray.cpp

HEADERS += \
    qttypeprinters.h \
    util.h

INSTALLS =
