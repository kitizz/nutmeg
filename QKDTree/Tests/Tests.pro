#-------------------------------------------------
#
# Project created by QtCreator 2013-01-28T16:28:03
#
#-------------------------------------------------

QT       += testlib

TARGET = tst_QKDTreeTests
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_QKDTreeTests.cpp \
    main.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"


#Linkage for QKDTree library
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../QKDTree/release/ -lQKDTree
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../QKDTree/debug/ -lQKDTree
else:unix: LIBS += -L$$OUT_PWD/../QKDTree/ -lQKDTree

INCLUDEPATH += $$PWD/../QKDTree
DEPENDPATH += $$PWD/../QKDTree

HEADERS += \
    tst_QKDTreeTests.h

#Linkage for QVectorND library
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../QVectorND/release/ -lQVectorND
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../QVectorND/debug/ -lQVectorND
else:unix: LIBS += -L$$OUT_PWD/../QVectorND/ -lQVectorND

INCLUDEPATH += $$PWD/../QVectorND
DEPENDPATH += $$PWD/../QVectorND
