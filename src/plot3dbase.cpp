#include "plot3dbase.h"

Plot3DBase::Plot3DBase(QQuickItem *parent)
    : QQuickItem(parent)
    , m_axis(0)
    , m_handle("")
    , m_matrix(new Qt3D::QMatrixTransform())  // Identity
    , m_transform(new Qt3D::QTransform())
{
    m_entity = new QEntity();
    m_transform->addTransform(m_matrix);
    m_entity->addComponent(m_transform);
}

Axis3DBase *Plot3DBase::axis() const
{
    return m_axis;
}

void Plot3DBase::setAxis(Axis3DBase *axis)
{
    if (m_axis == axis)
        return;

    m_axis = axis;
    emit axisChanged(axis);
}

QString Plot3DBase::handle() const
{
    return m_handle;
}

void Plot3DBase::setHandle(QString handle)
{
    if (m_handle == handle)
        return;

    m_handle = handle;
    emit handleChanged(handle);
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

void Plot3DBase::updateAxis()
{
    // Work up the tree until the next Axis item is found.
    QObject *newParent = parent();
    Axis3DBase *axis;
    while (newParent) {
        axis = qobject_cast<Axis3DBase*>(newParent);
        if (axis) break;
        newParent = newParent->parent();
    }

    setAxis(axis);
}
