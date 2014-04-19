#include <QApplication>
#include <QQuickView>
#include <QQmlProperty>
#include "mainwindow.h"

#include "lineplot.h"
#include "lineplotcanvas.h"
#include "axisbase.h"
#include "locators.h"
#include "linespec.h"
#include "axiscanvas2d.h"
#include "figurebase.h"
#include "server.h"
#include "controller.h"

#include "fileio.h"

int main(int argc, char *argv[])
{
    qmlRegisterType<FigureBase>("Graphr", 1,0, "FigureBase");

    qmlRegisterType<AxisBase>("Graphr", 1,0, "AxisBase");
    qmlRegisterType<AxisCanvas2D>("Graphr", 1,0, "AxisCanvas2D");
    qmlRegisterType<AxisSpec>("Graphr", 1,0, "AxisSpec");
    qmlRegisterType<AxisGrid>("Graphr", 1,0, "AxisGrid");
    qmlRegisterType<AxisMargins>("Graphr", 1,0, "AxisMargins");
    qmlRegisterType<AutoLocator>("Graphr", 1,0, "AutoLocator");
    qmlRegisterType<HardLocator>("Graphr", 1,0, "HardLocator");
    qmlRegisterType<SpacedLocator>("Graphr", 1,0, "SpacedLocator");

    qmlRegisterType<LinePlot>("Graphr", 1,0, "LinePlotBase");
    qmlRegisterType<LinePlotCanvas>("Graphr", 1,0, "LinePlotCanvas");
    qmlRegisterType<LineSpec>("Graphr", 1,0, "LineSpec");

    qmlRegisterType<Server>("Graphr", 1,0, "ServerBase");
    qmlRegisterType<Controller>("Graphr", 1,0, "ControllerBase");

    qmlRegisterType<FileIO>("FileIO", 1,0, "FileIO");

    QApplication app(argc, argv);

    MainWindow *w = new MainWindow(QUrl("qrc:/qml/main.qml"));
    w->show();

    QQuickView *view = w->view;

    Server* server = view->rootObject()->findChild<Server*>("server");
    if (server) {
        server->setMainWindow(w);
        server->start();
    }

    return app.exec();
}
