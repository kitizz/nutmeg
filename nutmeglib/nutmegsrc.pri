DISTFILES += \
    shaders/point.vert

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += \
    ../3rdparty/nzmqt/nzmqt.cpp \
    ../3rdparty/nzmqt/zmq.hpp \
    src/common/figurebase.cpp \
    src/common/nutmegobject.cpp \
    src/common/locators.cpp \
    src/common/linespec.cpp \
    src/common/fillspec.cpp \
    src/common/axisspec.cpp \
    src/common/axisbase.cpp \
    src/common/plotbase.cpp \
    src/common/defaults.cpp \
    src/common/guiitem.cpp \
    src/util/mouseevent.cpp \
    src/util/util.cpp \
    src/util/ndarray.cpp \
    src/layout/layoutgrid.cpp \
    src/layout/layoutcolumn.cpp \
    src/layout/layoutrow.cpp \
    src/server/server.cpp \
    src/server/controller.cpp \
    src/server/pushsocket.cpp \
    src/server/asyncqueue.cpp \
    src/2d/plots/lineplot.cpp \
    src/2d/plots/plotcanvas.cpp \
    src/2d/plots/xyplot.cpp \
    src/2d/plots/imagecanvas.cpp \
    src/2d/plots/imageplot.cpp \
    src/2d/plots/shapecanvas.cpp \
    src/2d/plots/canvasplot.cpp \
    src/2d/plots/canvasshape.cpp \
    src/2d/plots/canvasline.cpp \
    src/2d/plots/canvasplotcanvas.cpp \
    src/2d/plots/canvasrect.cpp \
    src/2d/plots/canvastext.cpp \
    src/2d/axes/axis2dbase.cpp \
    src/2d/plots/plot2dbase.cpp \
    src/2d/plots/linesegmentplot.cpp \
    src/2d/plots/linesegmentplotcanvas.cpp \
    src/2d/plots/streamingplot.cpp \
    src/3d/axes/axis3dbase.cpp \
    src/3d/plots/plot3dbase.cpp \
    src/common/guibase.cpp \
    src/util/arrayutil.cpp \
    src/nutmeglib_plugin.cpp \
    $$PWD/src/3d/plots/pointcloudgeometry.cpp \
    $$PWD/src/3d/plots/pointcloudplot.cpp \
    $$PWD/src/3d/plots/raycloudplot.cpp \
    $$PWD/src/3d/plots/raycloudgeometry.cpp \
    $$PWD/src/2d/axes/axiscanvas2d.cpp \
    $$PWD/src/common/nutmegrenderer.cpp \
    $$PWD/src/2d/plots/lineplotrenderer.cpp \
    $$PWD/src/2d/axes/axis2drenderer.cpp

HEADERS += \
    src/nutmeglibdecl.h \
    src/nutmeglib_plugin.h \
    ../3rdparty/nzmqt/nzmqt.hpp \
    src/common/figurebase.h \
    src/common/nutmegobject.h \
    src/common/locators.h \
    src/common/linespec.h \
    src/common/fillspec.h \
    src/common/axisspec.h \
    src/common/axisbase.h \
    src/common/plotbase.h \
    src/common/defaults.h \
    src/common/guiitem.h \
    src/util/mouseevent.h \
    src/util/util.h \
    src/util/ndarray.h \
    src/util/arrayutil.h \
    src/layout/layoutgrid.h \
    src/layout/layoutcolumn.h \
    src/layout/layoutrow.h \
    src/server/server.h \
    src/server/controller.h \
    src/server/pushsocket.h \
    src/server/task.h \
    src/server/server_util.h \
    src/server/asyncqueue.h \
    src/server/nutmegerror.h \
    src/2d/plots/lineplot.h \
    src/2d/plots/plotcanvas.h \
    src/2d/plots/xyplot.h \
    src/2d/plots/imagecanvas.h \
    src/2d/plots/imageplot.h \
    src/2d/plots/shapecanvas.h \
    src/2d/plots/canvasplot.h \
    src/2d/plots/canvasshape.h \
    src/2d/plots/canvasline.h \
    src/2d/plots/canvasplotcanvas.h \
    src/2d/plots/canvasrect.h \
    src/2d/plots/canvastext.h \
    src/2d/axes/axis2dbase.h \
    src/2d/plots/plot2dbase.h \
    src/2d/plots/linesegmentplot.h \
    src/2d/plots/linesegmentplotcanvas.h \
    src/2d/plots/streamingplot.h \
    src/3d/axes/axis3dbase.h \
    src/3d/plots/plot3dbase.h \
    src/server/binaryspec.h \
    src/common/guibase.h \
    $$PWD/src/3d/plots/pointcloudgeometry.h \
    $$PWD/src/3d/plots/pointcloudplot.h \
    $$PWD/src/3d/plots/raycloudplot.h \
    $$PWD/src/3d/plots/raycloudgeometry.h \
    $$PWD/src/2d/axes/axiscanvas2d.h \
    $$PWD/src/common/nutmegrenderer.h \
    $$PWD/src/2d/plots/lineplotrenderer.h \
    $$PWD/src/2d/axes/axis2drenderer.h

RESOURCES += \
    $$PWD/nutmeglib.qrc
