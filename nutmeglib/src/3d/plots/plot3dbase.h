#ifdef SUPPORT_3D
#ifndef PLOT3DBASE_H
#define PLOT3DBASE_H

#include <QQuickItem>

#include "../../common/nutmegobject.h"
#include "../axes/axis3dbase.h"

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>

using namespace Qt3DCore;

class Axis3DBase;
class NUTMEGLIB_EXPORT Plot3DBase : public PlotBase
{
    Q_OBJECT
    Q_PROPERTY(QEntity* entity READ entity NOTIFY entityChanged)
    Q_PROPERTY(QMatrix4x4 transform READ transform WRITE setTransform NOTIFY transformChanged)

public:
    explicit Plot3DBase(QQuickItem *parent = 0);

    Axis3DBase* axis3d() const;

    QEntity *entity() const;

    QMatrix4x4 transform() const;
    void setTransform(QMatrix4x4 transform);

signals:
    void entityChanged(QEntity* entity);
    void transformChanged(QMatrix4x4 transform);

public slots:
    virtual void updateAxis(AxisBase* oldAxis, AxisBase* newAxis);
//    virtual void print(QPainter *painter);

private:
    void setAxis(Axis3DBase* axis);
    Axis3DBase* m_axis;
    QEntity* m_entity;
//    Qt3DCore::QMatrixTransform *m_matrix;
    Qt3DCore::QTransform *m_transform;
};

#endif // PLOT3DBASE_H
#endif
