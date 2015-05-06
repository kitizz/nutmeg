#include "plotcanvas.h"
#include <QGuiApplication>
#include <QScreen>

PlotCanvas::PlotCanvas(QQuickItem *parent)
    : QQuickPaintedItem(parent)
    , m_scaling(QGuiApplication::primaryScreen()->devicePixelRatio())
    , m_updateTriggered(true)
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
    update();
    m_updateTriggered = true;
}
