#ifdef SUPPORT_3D
#ifndef PLOT3DBASE_H
#define PLOT3DBASE_H

#include <QQuickItem>

#include "../../common/nutmegobject.h"
#include "../axes/axis3dbase.h"
#include "../../util/ndarray.h"

#include <Qt3DCore/QEntity>
#include <Qt3DRender/QMaterial>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QComponent>
#include <Qt3DRender/QParameter>

using namespace Qt3DCore;

class Axis3DBase;
class NUTMEGLIB_EXPORT Plot3DBase : public PlotBase
{
    Q_OBJECT
    Q_PROPERTY(Qt3DCore::QEntity* entity READ entity NOTIFY entityChanged)
//    Q_PROPERTY(Qt3DRender::QMaterial* material READ material NOTIFY materialChanged)
    Q_PROPERTY(QMatrix4x4 transform READ transform WRITE setTransform NOTIFY transformChanged)
    Q_PROPERTY(QList<qreal> dataLimits READ dataLimits NOTIFY dataLimitsChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

public:
    explicit Plot3DBase(QQuickItem *parent = 0);

    Axis3DBase* axis3d() const;

    QEntity *entity() const;

    QMatrix4x4 transform() const;
    void setTransform(QMatrix4x4 transform);

//    Qt3DRender::QMaterial *material() const;

    QList<qreal> dataLimits() const;

    QColor color() const;

signals:
    void entityChanged(QEntity* entity);
    void transformChanged(QMatrix4x4 transform);
//    void materialChanged(Qt3DRender::QMaterial* material);
    void dataLimitsChanged(QList<qreal> dataLimits);

    void colorChanged(QColor color);

public slots:
    virtual void updateAxis(AxisBase* oldAxis, AxisBase* newAxis);
    void addComponent(QComponent *comp);
    virtual void updateDataLimits();
//    virtual void print(QPainter *painter);

    void setColor(QColor color);

protected:
    void setDataLimits(QList<qreal> dataLimits);
    static NDArrayTyped<float> make3d(NDArray array, bool &success);
    Qt3DRender::QParameter *m_colorParam;

private:
//    void setAxis(Axis3DBase* axis);
    Axis3DBase* m_axis;
    QEntity* m_entity;
//    Qt3DCore::QMatrixTransform *m_matrix;
    Qt3DCore::QTransform *m_transform;
//    Qt3DRender::QMaterial *m_material;
    QList<qreal> m_dataLimits;
};

#endif // PLOT3DBASE_H
#endif
