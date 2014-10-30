#include <QApplication>
#include <QQuickView>
#include <QQmlProperty>
#include <QQmlContext>
#include "../mainwindow.h"
#include "../qmlwindow.h"

#include "lineplot.h"
#include "imageplot.h"
#include "lineplotcanvas.h"
#include "imagecanvas.h"
#include "axisbase.h"
#include "locators.h"
#include "linespec.h"
#include "axiscanvas2d.h"
#include "figurebase.h"
#include "server.h"
#include "controller.h"
#include "settings.h"

#include "fileio.h"

int main(int argc, char *argv[])
{
    qmlRegisterUncreatableType<QmlWindow>("Graphr", 1,0, "QmlWindow", "Cannot create QmlWindow");
    qmlRegisterUncreatableType<MainWindow>("Graphr", 1,0, "MainWindow", "Cannot create MainWindow");
    qmlRegisterType<FigureBase>("Graphr", 1,0, "FigureBase");

    qmlRegisterType<AxisBase>("Graphr", 1,0, "AxisBase");
    qmlRegisterType<AxisCanvas2D>("Graphr", 1,0, "AxisCanvas2D");
    qmlRegisterType<AxisSpec>("Graphr", 1,0, "AxisSpec");
    qmlRegisterType<AxisGrid>("Graphr", 1,0, "AxisGrid");
    qmlRegisterType<AxisMargins>("Graphr", 1,0, "AxisMargins");
    qmlRegisterType<AutoLocator>("Graphr", 1,0, "AutoLocator");
    qmlRegisterType<HardLocator>("Graphr", 1,0, "HardLocator");
    qmlRegisterType<SpacedLocator>("Graphr", 1,0, "SpacedLocator");

    qmlRegisterType<LineSpec>("Graphr", 1,0, "LineSpec");
    qmlRegisterType<LinePlot>("Graphr", 1,0, "LinePlotBase");
    qmlRegisterType<LinePlotCanvas>("Graphr", 1,0, "LinePlotCanvas");
    qmlRegisterType<ImagePlot>("Graphr", 1,0, "ImagePlotBase");
    qmlRegisterType<ImageCanvas>("Graphr", 1,0, "ImageCanvas");

    qmlRegisterType<Server>("Graphr", 1,0, "ServerBase");
    qmlRegisterType<Controller>("Graphr", 1,0, "ControllerBase");

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
