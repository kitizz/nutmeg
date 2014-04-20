#include "mainwindow.h"
#include <QDebug>

#include <QQmlEngine>
#include <QMenu>
#include <QAction>
#include <QSystemTrayIcon>
#include <QTimer>

MainWindow::MainWindow(QUrl qmlSource, QWidget *parent)
    : QmlWindow(qmlSource, true, parent)
    , m_server(0)
    , m_settingsWindow(0)
{
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

void MainWindow::createSystemTray()
{
    m_trayIcon = new QSystemTrayIcon(QIcon(":/images/icon.png"), this);

    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);

    QAction *quitAction = new QAction( "Exit", m_trayIcon);
    connect( quitAction, &QAction::triggered, [=](){
        if (m_settingsWindow)
            m_settingsWindow->exit();
        m_trayIcon->hide();
        this->exit();
    });

    QAction *hideAction = new QAction( "Show/Hide", m_trayIcon );
    connect( hideAction, &QAction::triggered, this, &QmlWindow::showHide);

    QAction *settingsAction = new QAction( "Settings...", m_trayIcon );
    connect( settingsAction, &QAction::triggered, this, &MainWindow::showSettings);

    QMenu *trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(settingsAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(hideAction);
    trayIconMenu->addAction(quitAction);

    m_trayIcon->setContextMenu(trayIconMenu);
    m_trayIcon->show();
}
