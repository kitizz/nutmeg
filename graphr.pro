# Add more folders to ship with the application, here
folder_01.source = qml/graphr
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

# C++11 !!
#QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS = -mmacosx-version-min=10.7 -std=gnu0x -stdlib=libc+
CONFIG +=c++11

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

DEFINES += \
# Uncomment this line for nzmqt "Include and Source File" compilation option.
    NZMQT_LIB

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    nzmqt/src/nzmqt/nzmqt.cpp \
    plotdata.cpp \
    server.cpp \
    plotbase.cpp \
    axisbase.cpp \
    figurebase.cpp \
    lineplot.cpp \
    controller.cpp \
    lineplotcanvas.cpp \
    plotcanvas.cpp \
    nutmegobject.cpp \
    mouseevent.cpp \
    QKDTree/QKDTree/QKDTree.cpp \
    QKDTree/QKDTree/QKDTreeDistanceMetric.cpp \
    QKDTree/QKDTree/QKDTreeNode.cpp \
    QKDTree/QVectorND/QVectorND.cpp \
    axiscanvas2d.cpp \
    locators.cpp \
    linespec.cpp \
    fileio.cpp

# Installation path
# target.path =

# Please do not modify the following two lines. Required for deployment.
include(qtquick2applicationviewer/qtquick2applicationviewer.pri)
qtcAddDeployment()

HEADERS += \
    nzmqt/include/nzmqt/nzmqt.hpp \
    plotdata.h \
    server.h \
    plotbase.h \
    axisbase.h \
    figurebase.h \
    lineplot.h \
    controller.h \
    lineplotcanvas.h \
    plotcanvas.h \
    nutmegobject.h \
    mouseevent.h \
    QKDTree/QKDTree/QKDTree.h \
    QKDTree/QKDTree/QKDTree_global.h \
    QKDTree/QKDTree/QKDTreeDistanceMetric.h \
    QKDTree/QKDTree/QKDTreeNode.h \
    QKDTree/QVectorND/QVectorND.h \
    QKDTree/QVectorND/QVectorND_global.h \
    axiscanvas2d.h \
    locators.h \
    linespec.h \
    fileio.h

LIBS += -lzmq

INCLUDEPATH += \
    /usr/local/include \
    nzmqt/include \
    nzmqt/externals/include

QMAKE_LIBDIR += \
    /usr/local/lib

OTHER_FILES += \
    qml/graphr/Column.qml


