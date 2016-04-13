#include "plotbase.h"

PlotBase::PlotBase(QQuickItem* parent)
    : QQuickItem(parent)
    , NutmegObject(this)
    , m_handle("plot")
    , m_axis(0)
    , m_antialias(false)
{
    connect(this, &QQuickItem::parentChanged, this, &PlotBase::findAxis);
}

QString PlotBase::handle() const
{
    return m_handle;
}

void PlotBase::setHandle(QString arg)
{
    if (m_handle == arg) return;
    m_handle = arg;
    emit handleChanged(arg);
}

AxisBase *PlotBase::axis() const
{
    return m_axis;
}

bool PlotBase::antialias() const
{
    return m_antialias;
}

void PlotBase::setAxis(AxisBase *arg)
{
    if (m_axis == arg) return;
    AxisBase* oldAxis = m_axis;
    m_axis = arg;
    updateAxis(oldAxis, m_axis);
    emit axisChanged(arg);
}

void PlotBase::findAxis()
{
    // Work up the tree until the next Axis item is found.
    QObject *newParent = parent();
    AxisBase *axis;
    while (newParent) {
        axis = qobject_cast<AxisBase*>(newParent);
        if (axis) break;
        newParent = newParent->parent();
    }

    setAxis(axis);
}

void PlotBase::setAntialias(bool antialias)
{
    if (m_antialias == antialias)
        return;

    m_antialias = antialias;
    emit antialiasChanged(antialias);
}
