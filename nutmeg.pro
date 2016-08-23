TEMPLATE = subdirs

#DEFINES += SUPPORT_3D

SUBDIRS = \
    app \
    nutmeglib \
    tests \
    googletest \

app.subdir = app
nutmeglib.subdir = nutmeglib
tests.subdir = tests
googletest.subdir = 3rdparty/GoogleTest

app.depends = nutmeglib
tests.depends = nutmeglib googletest

PROJECTNAME = nutmeg
VERSION = 0.1.0

include(defaults.pri)
