#include <QApplication>
#include <QQuickView>
#include <QQmlProperty>
#include <QQmlContext>
#include "mainwindow.h"
#include "qmlwindow.h"

#include "lineplot.h"
#include "lineplotcanvas.h"
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
    QmlWindow *settingsWindow = new QmlWindow(QUrl("qrc:/qml/Settings.qml"), true);
    settingsWindow->resize(300, 150);
    w->setSettingsWindow(settingsWindow);

    Settings *settings = new Settings();
    qDebug() << "Setting settings in main";

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
