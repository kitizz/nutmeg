TEMPLATE = lib
CONFIG += plugin

TARGET = nutmeglib
win32:DESTDIR = $$OUT_PWD

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
    qml/Gui/Layouts/Grid.qml \
    qml/Gui/Layouts/Column.qml \
    qml/Gui/Layouts/Row.qml \
    qml/Gui/Controls/ControlItem.qml \
    qml/Gui/Controls/Button.qml \
    qml/Gui/Controls/Slider.qml \

# Copy Plugin into its own directory (dynamic lib, QML, qmldir, etc)
installPath = $$OUT_PWD/../Nutmeg
#mkdirNutmeg.target = $$installPath
#mkdirNutmeg.commands = $(MKDIR) \"$$replace(installPath, /, $$QMAKE_DIR_SEP)\"
#QMAKE_EXTRA_TARGETS += mkdirNutmeg
#PRE_TARGETDEPS += $$mkdirNutmeg.target

qmlfiles.path = $$installPath
#qmlfile.depends = mkdirNutmeg
target.path = $$installPath
#target.depends = mkdirNutmeg

INSTALLS += target qmlfiles

# Copy Nutmeg.Gui stuff in /Gui directory
guiPath = $$installPath/Gui
#mkdirGui.target = $$guiPath
#mkdirGui.commands = $(MKDIR) \"$$replace(guiPath, /, $$QMAKE_DIR_SEP)\"
#mkdirGui.depends = mkdirNutmeg
#QMAKE_EXTRA_TARGETS += mkdirGui
#PRE_TARGETDEPS += $$mkdirGui.target

guifiles.path = $$guiPath
#guifiles.depends = mkdirGui
INSTALLS += guifiles

#qmlfiles.command = $(COPY_FILE) qmlfiles.files

unix:QMAKE_CXXFLAGS += -O3
win32:QMAKE_CXXFLAGS += /O2

DEFINES += _WINSOCKAPI_
DEFINES += NOMINMAX

# ZMQ Library stuff
unix:LIBS += -lzmq
win32:CONFIG(release, debug|release): LIBS += -L"$$zmq_dir\lib" -llibzmq
else:win32:CONFIG(debug, debug|release): LIBS += -L"$$zmq_dir\lib" -llibzmq_d

# Location of zmq include
unix:INCLUDEPATH += $$zmq_dir/include
win32:INCLUDEPATH += "$$zmq_dir\include"

## Location of zmq library
unix:QMAKE_LIBDIR += $$zmq_dir/lib
win32:QMAKE_LIBDIR += "$$zmq_dir\lib"

DISTFILES += \
    qml/Gui/qmldir

