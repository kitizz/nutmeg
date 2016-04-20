TEMPLATE = lib
CONFIG += plugin

TARGET = nutmeglib
DESTDIR = $$OUT_PWD/../Nutmeg

uri = com.kitizz.Nutmeg
DEFINES += NUTMEG_LIBRARY


include(../defaults.pri)
include(../config.pri)
include(nutmegsrc.pri)

QT += qml quick widgets core gui printsupport

# 3D Things. Must have Qt 5.6 or higher with Qt3D module.
DEFINES += SUPPORT_3D
QT += 3dcore 3drender 3dinput 3dquick

qmlfiles.files = \
    qml/qmldir \
    qml/MultiPointPinchArea.qml \
    qml/Figure.qml \
    qml/Axis.qml \
    qml/LinePlot.qml \
    qml/ImagePlot.qml \
    qml/CanvasPlot.qml \
    qml/Util.js \
    qml/Vector.js \

guifiles.files = \
    qml/Gui/qmldir \
    qml/Gui/Gui.qml \
    qml/Gui/Controls/ControlItem.qml \
    qml/Gui/Controls/Button.qml \
    qml/Gui/Controls/Slider.qml \

unix: {
    qmlfiles.commands =
    for (f, qmlfiles.files): qmlfiles.commands += $(COPY) $$PWD/$$f $$DESTDIR;
    qmlfiles.commands += $(MKDIR) $$DESTDIR/Gui;
    for (f, guifiles.files): qmlfiles.commands += $(COPY) $$PWD/$$f $$DESTDIR/Gui;
#    message($$qmlfiles.command)
}
win32: {
    dest = $$replace(DESTDIR, /, $$QMAKE_DIR_SEP)
    Fpaths =
    for (f, qmlfiles.files): Fpaths += $$replace(f, /, $$QMAKE_DIR_SEP)
    for (f, Fpaths): QMAKE_POST_LINK += $$quote(copy /y $$PWD/$$f $$dest;)

    QMAKE_POST_LINK += $$quote(if not exist $$dest\Gui mkdir $$dest\Gui; )

    Gpaths =
    for (f, guifiles.files): Gpaths += $$replace(f, /, $$QMAKE_DIR_SEP)
    for (f, Gpaths): QMAKE_POST_LINK += $$quote(copy /y $$PWD\$$f $$dest\Gui;)
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
else:win32:CONFIG(debug, debug|release): LIBS += -L"$$zmq_lib" -llibzmq_d

# Location of zmq include
unix:INCLUDEPATH += $$zmq_include
win32:INCLUDEPATH += "$$zmq_include"

## Location of zmq library
unix:QMAKE_LIBDIR += $$zmq_lib
win32:QMAKE_LIBDIR += "$$zmq_lib"

DISTFILES += \
    qml/Gui/qmldir

