#include "nutmeglib_plugin.h"
#include <QtQml/qqml.h>
#include <QMetaType>
#include <QList>

#include "common/guibase.h"
#include "common/guiitem.h"
#include "2d/plots/lineplot.h"
#include "2d/plots/streamingplot.h"
#include "2d/plots/linesegmentplot.h"
#include "2d/plots/imageplot.h"
#include "2d/plots/canvasplot.h"

#include "2d/plots/lineplotrenderer.h"
#include "2d/plots/linesegmentplotcanvas.h"
#include "2d/plots/imagecanvas.h"
#include "2d/plots/canvasplotcanvas.h"

#include "2d/plots/canvasline.h"
#include "2d/plots/canvasrect.h"
#include "2d/plots/canvastext.h"

#include "2d/axes/axis2dbase.h"
#include "common/locators.h"
#include "common/linespec.h"
#include "common/fillspec.h"
#include "common/figurebase.h"
#include "layout/layoutgrid.h"
#include "layout/layoutcolumn.h"
#include "layout/layoutrow.h"

#include "server/server.h"
#include "server/controller.h"
#include "server/nutmegerror.h"
#include "util/ndarray.h"

// 3D Stuff at the end
#include "3d/axes/axis3dbase.h"
#include "3d/plots/pointcloudplot.h"
#include "3d/plots/raycloudplot.h"

void NutmegPlugin::registerTypes(const char *uri)
{
    qDebug() << "RegisterTypes. Uri:" << uri;
    Q_ASSERT(uri == QLatin1String("Nutmeg"));

    int maj = 0, min = 1;

    qmlRegisterType<FigureBase>(uri, maj,min, "FigureBase");
    qmlRegisterType<GuiBase>(uri, maj,min, "GuiBase");
    qmlRegisterType<GuiItem>(uri, maj,min, "GuiItem");

#ifdef SUPPORT_3D
    qmlRegisterType<Axis3DBase>(uri, maj,min, "Axis3DBase");
//    qmlRegisterType<PointCloudPlot>(uri, maj,min, "PointCloudBase");
//    qmlRegisterType<RayCloudPlot>(uri, maj,min, "RayCloudBase");
#endif

    // ---------- Layout Stuff --------------
    qmlRegisterType<Axis2DBase>(uri, maj,min, "Axis2DBase");
//    qmlRegisterType<AxisCanvas2D>(uri, maj,min, "AxisCanvas2D");
    qmlRegisterType<AxisSpec>(uri, maj,min, "AxisSpec");
    qmlRegisterType<AxisGrid>(uri, maj,min, "AxisGrid");
    qmlRegisterType<AxisMargins>(uri, maj,min, "AxisMargins");
    qmlRegisterType<AutoLocator>(uri, maj,min, "AutoLocator");
    qmlRegisterType<HardLocator>(uri, maj,min, "HardLocator");
    qmlRegisterType<SpacedLocator>(uri, maj,min, "SpacedLocator");

    qmlRegisterType<LayoutGrid>(uri, maj,min, "Grid");
    qmlRegisterType<LayoutColumn>(uri, maj,min, "Column");
    qmlRegisterType<LayoutRow>(uri, maj,min, "Row");

    // ---------- Plotting stuff ------------
    qmlRegisterType<LineSpec>(uri, maj,min, "LineSpec");
    qmlRegisterType<FillSpec>(uri, maj,min, "FillSpec");

    qmlRegisterType<LinePlot>(uri, maj,min, "LinePlotBase");
//    qmlRegisterType<StreamingPlot>(uri, maj,min, "StreamingPlotBase");
//    qmlRegisterType<LineSegmentPlot>(uri, maj,min, "LineSegmentPlotBase");
//    qmlRegisterType<ImagePlot>(uri, maj,min, "ImagePlotBase");
//    qmlRegisterType<CanvasPlot>(uri, maj,min, "CanvasPlotBase");

//    qmlRegisterType<LinePlotCanvas>(uri, maj,min, "LinePlotCanvas");
//    qmlRegisterType<LineSegmentPlotCanvas>(uri, maj,min, "LineSegmentPlotCanvas");
//    qmlRegisterType<ImageCanvas>(uri, maj,min, "ImageCanvas");
//    qmlRegisterType<CanvasPlotCanvas>(uri, maj,min, "CanvasPlotCanvas");

    qmlRegisterType<CanvasLine>(uri, maj,min, "CanvasLine");
    qmlRegisterType<CanvasRect>(uri, maj,min, "CanvasRect");
    qmlRegisterType<CanvasText>(uri, maj,min, "CanvasText");

    // ----------- Infrastructure ------------
    qmlRegisterType<Server>(uri, maj,min, "ServerBase");
    qmlRegisterType<Controller>(uri, maj,min, "ControllerBase");

    qRegisterMetaType<NutmegError>("NutmegError");
    qRegisterMetaType<Task>("Task");
    qRegisterMetaType<NutmegObject*>("NutmegObject*");

    QMetaType::registerConverter< QList<qreal>, NDArray >();
    QMetaType::registerConverter< QVariantList, NDArray >();
}
