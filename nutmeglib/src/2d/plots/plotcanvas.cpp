#include "plotcanvas.h"
#include <QGuiApplication>
#include <QScreen>

PlotCanvas::PlotCanvas(PlotBase *parent)
    : QQuickFramebufferObject::Renderer()
    , m_parent(parent)
    , m_dirtyData(true)
    , m_scaling(qApp->devicePixelRatio())
    , m_updateTriggered(true)
{
    if (parent)
        parent->connect(parent, &PlotBase::dataChanged, [=](){ this->dataChanged(); } );
}

PlotBase *PlotCanvas::parent() const
{
    return m_parent;
}

void PlotCanvas::dataChanged()
{
    m_dirtyData = true;
}
