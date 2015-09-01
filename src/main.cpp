#include <QApplication>
#include <QQuickView>
#include <QQmlProperty>
#include <QQmlContext>
#include "../mainwindow.h"
#include "../qmlwindow.h"

#include "lineplot.h"
#include "linesegmentplot.h"
#include "imageplot.h"
#include "canvasplot.h"

#include "lineplotcanvas.h"
#include "linesegmentplotcanvas.h"
#include "imagecanvas.h"
#include "canvasplotcanvas.h"

#include "canvasline.h"
#include "canvasrect.h"
#include "canvastext.h"

#include "axis2dbase.h"
#include "locators.h"
#include "linespec.h"
#include "fillspec.h"
#include "axiscanvas2d.h"
#include "figurebase.h"
#include "layoutgrid.h"
#include "layoutcolumn.h"
#include "layoutrow.h"

#include "server.h"
#include "controller.h"
#include "settings.h"

#include "fileio.h"

// 3D Stuff at the end
#include "axis3dbase.h"
#include "pointcloud.h"

int main(int argc, char *argv[])
{
    qmlRegisterUncreatableType<QmlWindow>("Nutmeg", 1,0, "QmlWindow", "Cannot create QmlWindow");
    qmlRegisterUncreatableType<MainWindow>("Nutmeg", 1,0, "MainWindow", "Cannot create MainWindow");
    qmlRegisterType<FigureBase>("Nutmeg", 1,0, "FigureBase");

#ifdef SUPPORT_3D
    qmlRegisterType<Axis3DBase>("Nutmeg", 1,0, "Axis3DBase");
    qmlRegisterType<PointCloud>("Nutmeg", 1,0, "PointCloudBase");
#endif

    // ---------- Layout Stuff --------------
    qmlRegisterType<Axis2DBase>("Nutmeg", 1,0, "Axis2DBase");
    qmlRegisterType<AxisCanvas2D>("Nutmeg", 1,0, "AxisCanvas2D");
    qmlRegisterType<AxisSpec>("Nutmeg", 1,0, "AxisSpec");
    qmlRegisterType<AxisGrid>("Nutmeg", 1,0, "AxisGrid");
    qmlRegisterType<AxisMargins>("Nutmeg", 1,0, "AxisMargins");
    qmlRegisterType<AutoLocator>("Nutmeg", 1,0, "AutoLocator");
    qmlRegisterType<HardLocator>("Nutmeg", 1,0, "HardLocator");
    qmlRegisterType<SpacedLocator>("Nutmeg", 1,0, "SpacedLocator");

    qmlRegisterType<LayoutGrid>("Nutmeg", 1,0, "Grid");
    qmlRegisterType<LayoutColumn>("Nutmeg", 1,0, "Column");
    qmlRegisterType<LayoutRow>("Nutmeg", 1,0, "Row");

    // ---------- Plotting stuff ------------
    qmlRegisterType<LineSpec>("Nutmeg", 1,0, "LineSpec");
    qmlRegisterType<FillSpec>("Nutmeg", 1,0, "FillSpec");

    qmlRegisterType<LinePlot>("Nutmeg", 1,0, "LinePlotBase");
    qmlRegisterType<LineSegmentPlot>("Nutmeg", 1,0, "LineSegmentPlotBase");
    qmlRegisterType<ImagePlot>("Nutmeg", 1,0, "ImagePlotBase");
    qmlRegisterType<CanvasPlot>("Nutmeg", 1,0, "CanvasPlotBase");

    qmlRegisterType<LinePlotCanvas>("Nutmeg", 1,0, "LinePlotCanvas");
    qmlRegisterType<LineSegmentPlotCanvas>("Nutmeg", 1,0, "LineSegmentPlotCanvas");
    qmlRegisterType<ImageCanvas>("Nutmeg", 1,0, "ImageCanvas");
    qmlRegisterType<CanvasPlotCanvas>("Nutmeg", 1,0, "CanvasPlotCanvas");

    qmlRegisterType<CanvasLine>("Nutmeg", 1,0, "CanvasLine");
    qmlRegisterType<CanvasRect>("Nutmeg", 1,0, "CanvasRect");
    qmlRegisterType<CanvasText>("Nutmeg", 1,0, "CanvasText");

    // ----------- Infrastructure ------------
    qmlRegisterType<Server>("Nutmeg", 1,0, "ServerBase");
    qmlRegisterType<Controller>("Nutmeg", 1,0, "ControllerBase");

    qmlRegisterType<FileIO>("FileIO", 1,0, "FileIO");

    QApplication app(argc, argv);

    MainWindow *w = new MainWindow(QUrl("qrc:/qml/main.qml"));
    w->connect(&app, &QApplication::aboutToQuit, w, &QmlWindow::exit);

    QmlWindow *settingsWindow = new QmlWindow(QUrl("qrc:/qml/Settings.qml"), true);
    settingsWindow->resize(300, 150);
    w->setSettingsWindow(settingsWindow);

    QmlWindow *aboutWindow = new QmlWindow(QUrl("qrc:/qml/About.qml"), true);
    aboutWindow->resize(500, 400);
    w->setAboutWindow(aboutWindow);

    // Settings is the common object between the different windows.
    Settings *settings = new Settings();
    w->view()->rootObject()->setProperty("settings", QVariant::fromValue(settings));
    settingsWindow->view()->rootObject()->setProperty("settings", QVariant::fromValue(settings));

    Server* server = w->view()->rootObject()->findChild<Server*>("server");
    if (server) {
        w->setServer(server);
        server->start();
    }

    w->show();
    return app.exec();
}
