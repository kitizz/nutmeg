#include "mainwindow.h"
#include <QDebug>

#include <QQmlEngine>
#include <QMenu>
#include <QAction>
#include <QSystemTrayIcon>
#include <QTimer>

#ifdef Q_OS_MACX
    void qt_mac_set_dock_menu(QMenu *menu);
#endif

MainWindow::MainWindow(QUrl qmlSource, QWidget *parent)
    : QmlWindow(qmlSource, true, parent)
    , m_server(0)
    , m_settingsWindow(0)
{
    setWindowIcon(QIcon(":/images/icon.png"));
    createSystemTray();
    this->resize(700, 500);
}

Server *MainWindow::server() const
{
    return m_server;
}

void MainWindow::setServer(Server *arg)
{
    if (m_server == arg) return;
    m_server = arg;
    if (m_server)
        m_server->setMainWindow(this);
    emit serverChanged(arg);
}

QmlWindow *MainWindow::settingsWindow() const
{
    return m_settingsWindow;
}

void MainWindow::setSettingsWindow(QmlWindow *arg)
{
    if (m_settingsWindow == arg) return;
    m_settingsWindow = arg;
    emit settingsWindowChanged(arg);
}

QmlWindow *MainWindow::aboutWindow() const
{
    return m_aboutWindow;
}

void MainWindow::setAboutWindow(QmlWindow *arg)
{
    if (m_aboutWindow == arg) return;
    m_aboutWindow = arg;
    emit aboutWindowChanged(arg);
}

void MainWindow::notify(const QString &title, const QString &msg)
{
    m_trayIcon->showMessage(title, msg);
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason && reason != QSystemTrayIcon::DoubleClick)
        return;
    showHide();
}

void MainWindow::showSettings()
{
    if (m_settingsWindow)
        m_settingsWindow->show();
}

void MainWindow::showAbout()
{
    if (m_aboutWindow)
        m_aboutWindow->show();
}

void MainWindow::createSystemTray()
{
    // TODO: Find a way to make the icon not pixelated on high-dpi diplays.
    QIcon icon;
    icon.addFile(":/images/logo.icns");
    m_trayIcon = new QSystemTrayIcon(icon, this);

    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);

    QAction *quitAction = new QAction( "Exit", m_trayIcon);
    connect( quitAction, &QAction::triggered, [=](){
        if (m_settingsWindow)
            m_settingsWindow->exit();
        if (m_aboutWindow)
            m_aboutWindow->exit();
        m_trayIcon->hide();
        this->exit();
    });

    QAction *hideAction = new QAction( "Show/Hide", m_trayIcon );
    connect( hideAction, &QAction::triggered, this, &QmlWindow::showHide);

    QAction *aboutAction = new QAction( "About...", m_trayIcon );
    connect( aboutAction, &QAction::triggered, this, &MainWindow::showAbout);

    QAction *settingsAction = new QAction( "Settings...", m_trayIcon );
    connect( settingsAction, &QAction::triggered, this, &MainWindow::showSettings);

    QMenu *trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(settingsAction);
    trayIconMenu->addAction(aboutAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(hideAction);
    trayIconMenu->addAction(quitAction);

    // This makes it easier to access menu actions if the system tray icon gets
    // lost behind a long task bar...
#ifdef Q_OS_MACX
    QMenu *dockMenu = new QMenu(this);
    dockMenu->addAction(settingsAction);
    dockMenu->addAction(aboutAction);
    qt_mac_set_dock_menu(dockMenu);
#endif

    m_trayIcon->setContextMenu(trayIconMenu);
    m_trayIcon->show();
}
