# Add more folders to ship with the application, here
#folder_01.source = qml/nutmeg
#folder_01.target = qml
#DEPLOYMENTFOLDERS = folder_01
TEMPLATE = app

# C++11 !!
#QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS = -mmacosx-version-min=10.7 -std=gnu0x -stdlib=libc+
CONFIG +=c++11

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

QT += qml quick widgets core gui quickwidgets

DEFINES += \
    NZMQT_LIB

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += src/main.cpp \
    nzmqt/nzmqt.cpp \
    QKDTree/QKDTree/QKDTree.cpp \
    QKDTree/QKDTree/QKDTreeDistanceMetric.cpp \
    QKDTree/QKDTree/QKDTreeNode.cpp \
    QKDTree/QVectorND/QVectorND.cpp \
    src/plotdata.cpp \
    src/server.cpp \
    src/plotbase.cpp \
    src/axisbase.cpp \
    src/figurebase.cpp \
    src/lineplot.cpp \
    src/controller.cpp \
    src/lineplotcanvas.cpp \
    src/plotcanvas.cpp \
    src/nutmegobject.cpp \
    src/mouseevent.cpp \
    src/axiscanvas2d.cpp \
    src/locators.cpp \
    src/linespec.cpp \
    src/fileio.cpp \
    mainwindow.cpp

# Please do not modify the following two lines. Required for deployment.
#include(qtquick2applicationviewer/qtquick2applicationviewer.pri)
#qtcAddDeployment()

HEADERS += \
    nzmqt/nzmqt.hpp \
    QKDTree/QKDTree/QKDTree.h \
    QKDTree/QKDTree/QKDTree_global.h \
    QKDTree/QKDTree/QKDTreeDistanceMetric.h \
    QKDTree/QKDTree/QKDTreeNode.h \
    QKDTree/QVectorND/QVectorND.h \
    QKDTree/QVectorND/QVectorND_global.h \
    src/plotdata.h \
    src/server.h \
    src/plotbase.h \
    src/axisbase.h \
    src/figurebase.h \
    src/lineplot.h \
    src/controller.h \
    src/lineplotcanvas.h \
    src/plotcanvas.h \
    src/nutmegobject.h \
    src/mouseevent.h \
    src/axiscanvas2d.h \
    src/locators.h \
    src/linespec.h \
    src/fileio.h \
    mainwindow.h

LIBS += -lzmq

INCLUDEPATH += \
    /usr/local/include \ # Location of zmq include
    nzmqt/

QMAKE_LIBDIR += \
    /usr/local/lib # Location of zmq library

RESOURCES += qml.qrc

# Default rules for deployment.
include(deployment.pri)

FORMS +=


