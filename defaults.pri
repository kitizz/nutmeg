# C++11 !!
win32 {
    QMAKE_CXXFLAGS += -std=c++11
}

unix:!macx {
    QMAKE_CXXFLAGS += -std=c++11
}

macx {
    QMAKE_CXXFLAGS = -mmacosx-version-min=10.9 -std=gnu0x -stdlib=libc++
}

CONFIG += c++11
