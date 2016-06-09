#include <QApplication>
#include <QQuickView>
#include <QQuickItem>
#include <QQmlProperty>
#include <QQmlContext>
#include <QQmlEngine>
#include <QDebug>

#include "mainwindow.h"
#include "qmlwindow.h"

#include "settings.h"
#include "fileio.h"

int main(int argc, char *argv[])
{
    qmlRegisterUncreatableType<QmlWindow>("NutmegApp", 1,0, "QmlWindow", "Cannot create QmlWindow");
    qmlRegisterUncreatableType<MainWindow>("NutmegApp", 1,0, "MainWindow", "Cannot create MainWindow");
    qmlRegisterType<FileIO>("FileIO", 1,0, "FileIO");

    // Set up global app settings
//    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
//    format.setMajorVersion(3);
//    format.setMinorVersion(3);
//    format.setProfile(QSurfaceFormat::CoreProfile);
//    QSurfaceFormat::setDefaultFormat(format);

    QApplication::setAttribute(Qt::AA_SynthesizeMouseForUnhandledTouchEvents, false);

    // Make the app
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
    QVariant settings_var = QVariant::fromValue(settings);
    w->view()->rootObject()->setProperty("settings", settings_var);
    settingsWindow->view()->rootObject()->setProperty("settings", settings_var);

    w->show();
    return app.exec();
}
