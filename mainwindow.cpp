#include "mainwindow.h"
#include <QHBoxLayout>
#include <QQuickView>
#include <QQmlEngine>
#include <QDebug>

#include <QMenu>
#include <QAction>
#include <QSystemTrayIcon>
#include <QTimer>

MainWindow::MainWindow(QUrl qmlSource, QWidget *parent)
    : QWidget(parent)
    , view(new QQuickView())
    , m_lastSize(QSize())
    , m_exiting(false)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);

    view->connect(view->engine(), &QQmlEngine::quit, view, &QWindow::close);

    QWidget *container = QWidget::createWindowContainer(view);
    container->setMinimumSize(300, 300);
    container->setMaximumSize(60000, 60000);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setFocusPolicy(Qt::StrongFocus);
    view->setSource(qmlSource);
    view->setResizeMode(QQuickView::SizeRootObjectToView);

    layout->addWidget(container);

    createSystemTray();
}

bool MainWindow::event(QEvent *event)
{
    if (event->type() == QEvent::Close && !m_exiting) {
        qDebug() << "Event exit";
        onShowHide();
        event->setAccepted(false);
        return false;
    }
    return QWidget::event(event);
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason && reason != QSystemTrayIcon::DoubleClick)
        return;
    onShowHide();
}

void MainWindow::onShowHide()
{
    if (isVisible()) {
        m_lastSize = this->size();
        hide();
    } else {
        show();
        this->resize(m_lastSize);
        raise();
    }

}

void MainWindow::onExit()
{
    m_exiting = true;
    m_trayIcon->hide();
    delete view;
    this->close();
}

void MainWindow::createSystemTray()
{
    m_trayIcon = new QSystemTrayIcon(QIcon(":/images/icon.png"), this);

    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);

    QAction *quitAction = new QAction( "Exit", m_trayIcon);
    connect( quitAction, &QAction::triggered, this, &MainWindow::onExit);

    QAction *hideAction = new QAction( "Show/Hide", m_trayIcon );
    connect( hideAction, &QAction::triggered, this, &MainWindow::onShowHide);

    QMenu *trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(hideAction);
    trayIconMenu->addAction(quitAction);

    m_trayIcon->setContextMenu(trayIconMenu);
    m_trayIcon->show();
}
