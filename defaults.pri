# C++11 !!
unix:!macx {
    QMAKE_CXXFLAGS += -std=c++11
}

macx {
    QMAKE_CXXFLAGS = -mmacosx-version-min=10.9 -std=gnu0x -stdlib=libc++
}

CONFIG += c++11

CONFIG(release, debug|release): QMAKE_CXXFLAGS += -DQT_NO_DEBUG_OUTPUT
