#-------------------------------------------------
#
# Project created by QtCreator 2013-01-29T13:49:10
#
#-------------------------------------------------

TARGET = QVectorND
TEMPLATE = lib

DEFINES += QVECTORND_LIBRARY

SOURCES += \
    QVectorND.cpp

HEADERS +=\
        QVectorND_global.h \
    QVectorND.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
