#include "plotcanvas.h"
#include <QGuiApplication>
#include <QScreen>

PlotCanvas::PlotCanvas(QQuickItem *parent)
    : QQuickPaintedItem(parent)
    , m_scaling(qApp->devicePixelRatio())
    , m_updateTriggered(true)
{
//    m_scaling = QGuiApplication::primaryScreen()->devicePixelRatio();
    setRenderTarget(QQuickPaintedItem::FramebufferObject);
//    setRenderTarget(QQuickPaintedItem::Image);
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

void PlotCanvas::triggerOnMain()
{
    polish();
    update();
    m_updateTriggered = true;
}

void PlotCanvas::updateScale()
{
    qDebug() << "Scale updated" << QGuiApplication::primaryScreen()->devicePixelRatio();
    setScaling(QGuiApplication::primaryScreen()->devicePixelRatio());
}

bool PlotCanvas::updateTriggered()
{
    return m_updateTriggered;
}

void PlotCanvas::resetTrigger()
{
    m_updateTriggered = false;
}

void PlotCanvas::triggerUpdate()
{
    metaObject()->invokeMethod(this, "triggerOnMain", Qt::QueuedConnection);
}
