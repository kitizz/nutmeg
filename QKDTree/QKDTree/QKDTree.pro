#-------------------------------------------------
#
# Project created by QtCreator 2013-01-28T15:06:42
#
#-------------------------------------------------

QT += gui

TARGET = QKDTree
TEMPLATE = lib

DEFINES += QKDTREE_LIBRARY

SOURCES += QKDTree.cpp \
    QKDTreeNode.cpp \
    QKDTreeDistanceMetric.cpp

HEADERS += QKDTree.h\
        QKDTree_global.h \
    QKDTreeNode.h \
    QKDTreeDistanceMetric.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

#Linkage for QVectorND
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../QVectorND/release/ -lQVectorND
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../QVectorND/debug/ -lQVectorND
else:unix: LIBS += -L$$OUT_PWD/../QVectorND/ -lQVectorND

INCLUDEPATH += $$PWD/../QVectorND
DEPENDPATH += $$PWD/../QVectorND
