#include <QtGui/QGuiApplication>
#include <QQmlProperty>
#include "qtquick2applicationviewer.h"

#include "lineplot.h"
#include "lineplotcanvas.h"
#include "axisbase.h"
#include "figurebase.h"
#include "server.h"
#include "controller.h"

int main(int argc, char *argv[])
{
    qmlRegisterType<FigureBase>("Graphr", 1,0, "FigureBase");
    qmlRegisterType<AxisBase>("Graphr", 1,0, "AxisBase");
    qmlRegisterType<LinePlot>("Graphr", 1,0, "LinePlotBase");
    qmlRegisterType<LinePlotCanvas>("Graphr", 1,0, "LinePlotCanvas");
    qmlRegisterType<Server>("Graphr", 1,0, "ServerBase");
    qmlRegisterType<Controller>("Graphr", 1,0, "ControllerBase");

    QGuiApplication app(argc, argv);

    QtQuick2ApplicationViewer viewer;
    viewer.setMainQmlFile(QStringLiteral("qml/graphr/main.qml"));
    viewer.showExpanded();

    QQuickItem* server = viewer.rootObject()->findChild<QQuickItem*>("server");
    qDebug() << "Writing rootApp" << server;
    QQmlProperty::write(server, "rootApp", QVariant::fromValue(&app));

    return app.exec();
}
