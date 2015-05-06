#include "plotbase.h"

PlotBase::PlotBase(QQuickItem* parent)
    : QQuickItem(parent)
    , m_handle("plot")
    , m_axis(0)
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
