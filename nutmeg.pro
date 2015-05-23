# Add more folders to ship with the application, here
#folder_01.source = qml/nutmeg
#folder_01.target = qml
#DEPLOYMENTFOLDERS = folder_01
TEMPLATE = app

# C++11 !!
win32 {
    QMAKE_CXXFLAGS += -std=c++11
}

unix:!macx {
    QMAKE_CXXFLAGS += -std=c++11
}

macx {
    QMAKE_CXXFLAGS = -mmacosx-version-min=10.7 -std=gnu0x -stdlib=libc+
}

CONFIG += c++11

# Icon setup
macx:ICON = images/logo.icns

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

QT += qml quick widgets core gui printsupport

#DEFINES += NZMQT_LIB
DEFINES += QKDTREE_LIBRARY
DEFINES += QVECTORND_LIBRARY
DEFINES += _WINSOCKAPI_
DEFINES += NOMINMAX

# 3D Things. Must have Qt 5.5 or higher with Qt3D module.
DEFINES += SUPPORT_3D
QT += 3dcore 3drenderer 3dinput

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += src/main.cpp \
    nzmqt/nzmqt.cpp \
    nzmqt/zmq.hpp \
    QKDTree/QKDTree/QKDTree.cpp \
    QKDTree/QKDTree/QKDTreeDistanceMetric.cpp \
    QKDTree/QKDTree/QKDTreeNode.cpp \
    QKDTree/QVectorND/QVectorND.cpp \
    src/plotdata.cpp \
    src/server.cpp \
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
    mainwindow.cpp \
    src/pushsocket.cpp \
    qmlwindow.cpp \
    src/settings.cpp \
    src/xyplot.cpp \
    src/util.cpp \
    src/imagecanvas.cpp \
    src/imageplot.cpp \
    src/shapecanvas.cpp \
    src/canvasplot.cpp \
    src/canvasshape.cpp \
    src/canvasline.cpp \
    src/fillspec.cpp \
    src/canvasplotcanvas.cpp \
    src/canvasrect.cpp \
    src/canvastext.cpp \
    src/plot3dbase.cpp \
    src/axis3dbase.cpp \
    src/axisspec.cpp \
    src/pointcloud.cpp \
    src/axis2dbase.cpp \
    src/axisbase.cpp \
    src/plot2dbase.cpp \
    src/plotbase.cpp \
    src/defaults.cpp

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
    mainwindow.h \
    src/pushsocket.h \
    qmlwindow.h \
    src/settings.h \
    src/xyplot.h \
    src/util.h \
    src/imagecanvas.h \
    src/imageplot.h \
    src/shapecanvas.h \
    src/canvasplot.h \
    src/canvasshape.h \
    src/canvasline.h \
    src/fillspec.h \
    src/canvasplotcanvas.h \
    src/canvasrect.h \
    src/canvastext.h \
    src/plot3dbase.h \
    src/axis3dbase.h \
    src/axisspec.h \
    src/pointcloud.h \
    src/axis2dbase.h \
    src/axisbase.h \
    src/plot2dbase.h \
    src/plotbase.h \
    src/defaults.h

unix:LIBS += -lzmq
win32:CONFIG(release, debug|release): LIBS += -L"C:\zmq3.2.4\lib" -llibzmq
else:win32:CONFIG(debug, debug|release): LIBS += -L"C:\zmq3.2.4\lib" -llibzmq_d

# Location of zmq include
unix:INCLUDEPATH += /usr/local/include
win32:INCLUDEPATH += "C:\zmq3.2.4\include"

# Location of zmq library
unix:QMAKE_LIBDIR += /usr/local/lib
win32:QMAKE_LIBDIR += "C:\zmq3.2.4\lib"

RESOURCES += qml.qrc

# Default rules for deployment.
include(deployment.pri)

FORMS +=

OTHER_FILES += \
    nutmeg.qdocconf

DISTFILES += \
    shaders/point.vert


