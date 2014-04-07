#include "plotcanvas.h"
#include <QGuiApplication>
#include <QScreen>

PlotCanvas::PlotCanvas(QQuickItem *parent)
    : QQuickPaintedItem(parent)
    , m_scaling(QGuiApplication::primaryScreen()->devicePixelRatio())
{
//    m_scaling = QGuiApplication::primaryScreen()->devicePixelRatio();
    setRenderTarget(QQuickPaintedItem::FramebufferObject);
}

qreal PlotCanvas::scaling() const
{
    return m_scaling;
}

void PlotCanvas::setScaling(qreal arg)
{
    if (m_scaling == arg) return;
    m_scaling = arg;
    emit scalingChanged(arg);
}

QObject *PlotCanvas::app() const
{
    return m_app;
}

void PlotCanvas::setApp(QObject *arg)
{
    qDebug() << "Set app" << arg;
    if (m_app == arg) return;
    m_app = arg;
    if (m_app)
        m_app->installEventFilter(this);
    emit appChanged(arg);
}

bool PlotCanvas::eventFilter(QObject *watched, QEvent *event)
{
    qDebug() << event;
}
