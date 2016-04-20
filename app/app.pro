# Add more folders to ship with the application, here
#folder_01.source = qml/nutmeg
#folder_01.target = qml
#DEPLOYMENTFOLDERS = folder_01
TEMPLATE = app

# Default rules for deployment.
include(../deployment.pri)
# Default c++ flags
include(../defaults.pri)
include(../config.pri)

DESTDIR = $$OUT_PWD

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

QT += core gui widgets qml quick

# Platform dependent form of DESTDIR
dest = $$replace(DESTDIR, /, $$QMAKE_DIR_SEP)

# Including Pugin with app
# TODO: Work out this process for Windows, Linux, Android, iOS?
macx {
    # Icon setup
    ICON = ../images/logo.icns
    # Mac Stuff
    QMAKE_INFO_PLIST = Info.plist # qmake will copy this file to MyApp.app/Contents/Info.plist

    QMAKE_POST_LINK += $$quote(install_name_tool -change libnutmeglib.dylib @executable_path/../Resources/Nutmeg/libnutmeglib.dylib $$OUT_PWD/app.app/Contents/MacOS/app)

    nutlib.path = $$OUT_PWD/app.app/Contents/Resources
    zmq.files = $$zmq_lib/libzmq.dylib
}


win32 {
    nutlib.path = $$dest
    zmq.files = $$zmq_lib\libzmq.dll $$zmq_lib\libzmq_d.dll
}
unix:!macx {
    nutlib.path = $$dest
    zmq.files = $$zmq_lib/libzmq.so
}

# Copy Nutmeg lib in
nutmeg_dir = $$OUT_PWD/../Nutmeg
nutlib.files = $$replace(nutmeg_dir, /, $$QMAKE_DIR_SEP)
nutlib.commands += $(COPY_DIR) $$nutlib.files $$nutlib.path
export(nutlib.commands)
QMAKE_EXTRA_TARGETS += nutlib
POST_TARGETDEPS += nutlib

# Copy ZMQ in
for (f, zmq.files): zmq.commands += $(COPY) $$f $$dest;
export(zmq.commands)
QMAKE_EXTRA_TARGETS += zmq
POST_TARGETDEPS += zmq

# Headers and sources
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    qmlwindow.cpp \
    fileio.cpp \
    settings.cpp \

HEADERS += \
    mainwindow.h \
    qmlwindow.h \
    fileio.h \
    settings.h \

RESOURCES += \
    qml.qrc
