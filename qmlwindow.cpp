#include "qmlwindow.h"

#include <QHBoxLayout>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QDebug>

QmlWindow::QmlWindow(QUrl qmlSource, bool persistent, QWidget *parent, bool delayViewInit)
    : QWidget(parent)
    , m_view(0)
    , m_source(qmlSource)
    , m_lastSize(QSize())
    , m_exiting(false)
    , m_persistent(persistent)
{
    m_menuBar = new QMenuBar(0);

    m_view = new QQuickView();
    m_view->connect(m_view->engine(), &QQmlEngine::quit, m_view, &QWindow::close);

    if (!delayViewInit)
        finalizeView();
}

void QmlWindow::finalizeView()
{
    QWidget *container = QWidget::createWindowContainer(m_view);
    container->setMinimumSize(100, 100);
    container->setMaximumSize(60000, 60000);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setFocusPolicy(Qt::StrongFocus);

    m_view->setSource(m_source);
    m_view->setResizeMode(QQuickView::SizeRootObjectToView);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(container);
}

QMenuBar *QmlWindow::menuBar()
{
    return m_menuBar;
}

bool QmlWindow::event(QEvent *event)
{
    if (event->type() == QEvent::Close && !m_exiting && m_persistent) {
        qDebug() << "Event exit";
        showHide();
        event->setAccepted(false);
        return false;
    }
    return QWidget::event(event);
}


void QmlWindow::showHide()
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

void QmlWindow::exit()
{
    m_exiting = true;
    if (m_view) {
        delete m_view;
        m_view = 0;
    }
    this->close();
}
