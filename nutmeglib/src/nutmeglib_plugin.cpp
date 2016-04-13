#include "nutmeglib_plugin.h"
#include <QtQml/qqml.h>
#include <QMetaType>
#include <QList>

#include "common/guibase.h"
#include "common/guiitem.h"
#include "2d/plots/lineplot.h"
#include "2d/plots/linesegmentplot.h"
#include "2d/plots/imageplot.h"
#include "2d/plots/canvasplot.h"

#include "2d/plots/lineplotcanvas.h"
#include "2d/plots/linesegmentplotcanvas.h"
#include "2d/plots/imagecanvas.h"
#include "2d/plots/canvasplotcanvas.h"

#include "2d/plots/canvasline.h"
#include "2d/plots/canvasrect.h"
#include "2d/plots/canvastext.h"

#include "2d/axes/axis2dbase.h"
#include "2d/axes/axiscanvas2d.h"
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
#include "util/settings.h"

// 3D Stuff at the end
#include "3d/axes/axis3dbase.h"
#include "3d/plots/pointcloud.h"

void NutmegPlugin::registerTypes(const char *uri)
{
    qDebug() << "RegisterTypes. Uri:" << uri;
    Q_ASSERT(uri == QLatin1String("Nutmeg"));

    qmlRegisterType<FigureBase>(uri, 1,0, "FigureBase");
    qmlRegisterType<GuiBase>(uri, 1,0, "GuiBase");
    qmlRegisterType<GuiItem>(uri, 1,0, "GuiItem");

#ifdef SUPPORT_3D
    qmlRegisterType<Axis3DBase>(uri, 1,0, "Axis3DBase");
    qmlRegisterType<PointCloud>(uri, 1,0, "PointCloudBase");
#endif

    // ---------- Layout Stuff --------------
    qmlRegisterType<Axis2DBase>(uri, 1,0, "Axis2DBase");
    qmlRegisterType<AxisCanvas2D>(uri, 1,0, "AxisCanvas2D");
    qmlRegisterType<AxisSpec>(uri, 1,0, "AxisSpec");
    qmlRegisterType<AxisGrid>(uri, 1,0, "AxisGrid");
    qmlRegisterType<AxisMargins>(uri, 1,0, "AxisMargins");
    qmlRegisterType<AutoLocator>(uri, 1,0, "AutoLocator");
    qmlRegisterType<HardLocator>(uri, 1,0, "HardLocator");
    qmlRegisterType<SpacedLocator>(uri, 1,0, "SpacedLocator");

    qmlRegisterType<LayoutGrid>(uri, 1,0, "Grid");
    qmlRegisterType<LayoutColumn>(uri, 1,0, "Column");
    qmlRegisterType<LayoutRow>(uri, 1,0, "Row");

    // ---------- Plotting stuff ------------
    qmlRegisterType<LineSpec>(uri, 1,0, "LineSpec");
    qmlRegisterType<FillSpec>(uri, 1,0, "FillSpec");

    qmlRegisterType<LinePlot>(uri, 1,0, "LinePlotBase");
    qmlRegisterType<LineSegmentPlot>(uri, 1,0, "LineSegmentPlotBase");
    qmlRegisterType<ImagePlot>(uri, 1,0, "ImagePlotBase");
    qmlRegisterType<CanvasPlot>(uri, 1,0, "CanvasPlotBase");

    qmlRegisterType<LinePlotCanvas>(uri, 1,0, "LinePlotCanvas");
    qmlRegisterType<LineSegmentPlotCanvas>(uri, 1,0, "LineSegmentPlotCanvas");
    qmlRegisterType<ImageCanvas>(uri, 1,0, "ImageCanvas");
    qmlRegisterType<CanvasPlotCanvas>(uri, 1,0, "CanvasPlotCanvas");

    qmlRegisterType<CanvasLine>(uri, 1,0, "CanvasLine");
    qmlRegisterType<CanvasRect>(uri, 1,0, "CanvasRect");
    qmlRegisterType<CanvasText>(uri, 1,0, "CanvasText");

    // ----------- Infrastructure ------------
    qmlRegisterType<Server>(uri, 1,0, "ServerBase");
    qmlRegisterType<Controller>(uri, 1,0, "ControllerBase");

    qRegisterMetaType<NutmegError>("NutmegError");
    qRegisterMetaType<NutmegObject*>("NutmegObject*");

    QMetaType::registerConverter< QList<qreal>, NDArray >();
    QMetaType::registerConverter< QVariantList, NDArray >();
}
