#ifdef SUPPORT_3D
#include "plot3dbase.h"

Plot3DBase::Plot3DBase(QQuickItem *parent)
    : PlotBase(parent)
    , m_axis(0)
    , m_matrix(new Qt3D::QMatrixTransform())  // Identity
    , m_transform(new Qt3D::QTransform())
{
    m_entity = new QEntity();
    m_transform->addTransform(m_matrix);
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
    return m_matrix->matrix();
}

void Plot3DBase::setTransform(QMatrix4x4 transform)
{
    if (m_matrix->matrix() == transform)
        return;

    m_matrix->setMatrix(transform);
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
