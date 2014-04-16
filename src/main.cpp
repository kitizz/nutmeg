#include <QtGui/QGuiApplication>
#include <QQmlProperty>
#include "qtquick2applicationviewer.h"

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

    QGuiApplication app(argc, argv);

    QtQuick2ApplicationViewer viewer;
    viewer.setMainQmlFile(QStringLiteral("qml/graphr/main.qml"));
    viewer.showExpanded();
    viewer.setWidth(700);
    viewer.setHeight(450);

    QQuickItem* server = viewer.rootObject()->findChild<QQuickItem*>("server");
    qDebug() << "Writing rootApp" << server;
    QQmlProperty::write(server, "rootApp", QVariant::fromValue(&app));

    return app.exec();
}
