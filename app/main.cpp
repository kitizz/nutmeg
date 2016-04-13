#include <QApplication>
#include <QQuickView>
#include <QQmlProperty>
#include <QQmlContext>
#include <QQmlEngine>
#include <QDebug>

#include "util/settings.h"
#include "server/server.h"
#include "server/server_util.h"

#include "mainwindow.h"
#include "qmlwindow.h"

#include "fileio.h"

int main(int argc, char *argv[])
{
    qmlRegisterUncreatableType<QmlWindow>("NutmegApp", 1,0, "QmlWindow", "Cannot create QmlWindow");
    qmlRegisterUncreatableType<MainWindow>("NutmegApp", 1,0, "MainWindow", "Cannot create MainWindow");
    qmlRegisterType<FileIO>("FileIO", 1,0, "FileIO");

    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_SynthesizeMouseForUnhandledTouchEvents, false);

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
    QVariant settings_var = QVariant::fromValue(settings);
    w->view()->rootObject()->setProperty("settings", settings_var);
    settingsWindow->view()->rootObject()->setProperty("settings", settings_var);

    Server* server = w->view()->rootObject()->findChild<Server*>("server");
    if (!server) {
        qFatal("Server object not instantiated! Exiting...");
        return 1;
    }

    // Two-way bind settings and server port numbers
    QObject::connect(server, &Server::portSubscribeChanged, settings, &Settings::setPortSubscribe);
    QObject::connect(server, &Server::portPublishChanged, settings, &Settings::setPortPublish);
    QObject::connect(settings, &Settings::portSubscribeChanged, server, &Server::setPortSubscribe);
    QObject::connect(settings, &Settings::portPublishChanged, server, &Server::setPortPublish);

    server->setApp(&app);
    w->setServer(server);
    server->start();

    w->show();
    return app.exec();
}
