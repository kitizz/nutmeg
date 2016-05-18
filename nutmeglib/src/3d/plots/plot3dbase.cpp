#ifdef SUPPORT_3D
#include "plot3dbase.h"

#include <QGeometryRenderer>

Plot3DBase::Plot3DBase(QQuickItem *parent)
    : PlotBase(parent)
    , m_axis(0)
    , m_transform(new Qt3DCore::QTransform())
{
    m_entity = new QEntity();
    m_entity->addComponent(m_transform);
}

Axis3DBase *Plot3DBase::axis3d() const
{
    return m_axis;
}

QEntity *Plot3DBase::entity() const
{
    return m_entity;
}

QMatrix4x4 Plot3DBase::transform() const
{
    return m_transform->matrix();
}

void Plot3DBase::setTransform(QMatrix4x4 transform)
{
    if (m_transform->matrix() == transform)
        return;

    m_transform->setMatrix(transform);
    emit transformChanged(transform);
}

void Plot3DBase::updateAxis(AxisBase *oldAxis, AxisBase *newAxis)
{
    if (oldAxis) {
        oldAxis->disconnect(this);
        oldAxis->deregisterPlot(this);
    }

    m_axis = qobject_cast<Axis3DBase*>(newAxis);
    if (m_axis) {
        m_axis->registerPlot(this);

    } else {
        qWarning() << Q_FUNC_INFO << "Plot is not a descendant of any Axis. It may not behave as expected.";
    }
}

#endif
