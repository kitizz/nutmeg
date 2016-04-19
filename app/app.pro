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

QTPLUGIN += nutmeglib
INCLUDEPATH += ../nutmeglib/src
LIBS += -L../nutmeglib -lnutmeglib

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

QT += core gui widgets qml quick

# Including Pugin with app
# TODO: Work out this process for Windows, Linux, Android, iOS?
macx {
    # Icon setup
    ICON = ../images/logo.icns
    # Mac Stuff
    QMAKE_INFO_PLIST = Info.plist # qmake will copy this file to MyApp.app/Contents/Info.plist

    resource = Contents/Resources/Nutmeg

#    libfiles.target = libfiles_target
    libfiles.files = $$files($$OUT_PWD/../Nutmeg/*)
    libfiles.path = $$resource
    QMAKE_BUNDLE_DATA += libfiles

    QMAKE_POST_LINK += $$quote(install_name_tool -change libnutmeglib.dylib @executable_path/../Resources/Nutmeg/libnutmeglib.dylib $$OUT_PWD/app.app/Contents/MacOS/app)
}

# Headers and sources
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    qmlwindow.cpp \
    fileio.cpp \

HEADERS += \
    mainwindow.h \
    qmlwindow.h \
    fileio.h \

RESOURCES += \
    qml.qrc
