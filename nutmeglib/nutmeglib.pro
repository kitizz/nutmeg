TEMPLATE = lib
CONFIG += plugin

TARGET = nutmeglib

ios {
    DESTDIR = $$OUT_PWD/../qml
}
!ios {
    DESTDIR = $$OUT_PWD/../Nutmeg
}

uri = com.kitizz.Nutmeg
DEFINES += NUTMEG_LIBRARY

include(../defaults.pri)
include(../config.pri)
include(nutmegsrc.pri)

QT += qml quick widgets core gui printsupport

# 3D Things. Must have Qt 5.6 or higher with Qt3D module.
#DEFINES += SUPPORT_3D
#QT += 3dcore 3drender 3dinput 3dquick

qmlfiles.files = \
    qml/qmldir \
    qml/MultiPointPinchArea.qml \
    qml/Figure.qml \
    qml/Axis.qml \
    qml/LinePlot.qml \
    qml/LineSegmentPlot.qml \
    qml/StreamingPlot.qml \
    qml/ImagePlot.qml \
    qml/CanvasPlot.qml \
    qml/Axis3D.qml \
    qml/Axis3DScene.qml \
    qml/PointCloud.qml \
    qml/RayCloud.qml \
    qml/GridMaterial.qml \
    qml/Grid3D.qml \
    qml/GridPlane.qml \
    qml/Util.js \
    qml/Vector.js \

guifiles.files = \
    qml/Gui/qmldir \
    qml/Gui/Gui.qml \
    qml/Gui/Controls/ControlItem.qml \
    qml/Gui/Controls/Button.qml \
    qml/Gui/Controls/Slider.qml \
    qml/Gui/Controls/Label.qml \
    qml/Gui/Controls/CheckBox.qml \

DISTFILES += $$qmlfiles.files \
DISTFILES += $$guifiles.files \
    qml/GridMaterial.qml \
    qml/Grid3D.qml \
    qml/GridPlane.qml \
    qml/RayCloud.qml \
    qml/Gui/Controls/Label.qml \
    qml/Gui/Controls/CheckBox.qml

unix: {
    for (f, qmlfiles.files): qmlfiles.commands += $(COPY) \"$$PWD/$$f\" \"$$DESTDIR\";
    qmlfiles.commands += $(MKDIR) $$DESTDIR/Gui;
    for (f, guifiles.files): qmlfiles.commands += $(COPY) \"$$PWD/$$f\" \"$$DESTDIR/Gui\";
#    message($$qmlfiles.command)
}
win32: {
    dest = $$replace(DESTDIR, /, $$QMAKE_DIR_SEP)
    pwd = $$replace(PWD, /, $$QMAKE_DIR_SEP)

    for (f, qmlfiles.files): Fpaths += $$replace(f, /, $$QMAKE_DIR_SEP)
    message($$Fpaths)
    for (f, Fpaths): qmlfiles.commands += $(COPY) \"$$pwd\\$$f\" \"$$dest\" &

    qmlfiles.commands += $(MKDIR) \"$$dest\\Gui\" &

    for (f, guifiles.files): Gpaths += $$replace(f, /, $$QMAKE_DIR_SEP)
    message($$Gpaths)
    for (f, Gpaths): qmlfiles.commands += $(COPY) \"$$pwd\\$$f\" \"$$dest\\Gui\" &
}

ios {
    CONFIG += qt c++11 static
    QMAKE_MOC_OPTIONS += -Muri=$$uri # static plugin must use this
}

first.depends = $(first) qmlfiles
export(first.depends)
export(qmlfiles.commands)
QMAKE_EXTRA_TARGETS += first qmlfiles

unix:QMAKE_CXXFLAGS += -O3
win32:QMAKE_CXXFLAGS += /O2

DEFINES += _WINSOCKAPI_
DEFINES += NOMINMAX

# ZMQ Library stuff
unix:LIBS += -lzmq
win32:CONFIG(release, debug|release): LIBS += -L"$$zmq_lib" -llibzmq
else:win32:CONFIG(debug, debug|release): LIBS += -L"$$zmq_lib" -llibzmq

# Location of zmq include
unix:INCLUDEPATH += $$zmq_include
win32:INCLUDEPATH += "$$zmq_include"

## Location of zmq library
unix:QMAKE_LIBDIR += $$zmq_lib
win32:QMAKE_LIBDIR += "$$zmq_lib"

DISTFILES +=


