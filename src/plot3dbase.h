#ifndef PLOT3DBASE_H
#define PLOT3DBASE_H

#include <QQuickItem>

#include "nutmegobject.h"
#include "axis3dbase.h"

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QMatrixTransform>

using namespace Qt3D;

class Axis3DBase;
class Plot3DBase : public QQuickItem, public NutmegObject
{
    Q_OBJECT
    Q_PROPERTY(Axis3DBase* axis READ axis NOTIFY axisChanged)
    Q_PROPERTY(QString handle READ handle WRITE setHandle NOTIFY handleChanged)

    Q_PROPERTY(QEntity* entity READ entity NOTIFY entityChanged)
    Q_PROPERTY(QMatrix4x4 transform READ transform WRITE setTransform NOTIFY transformChanged)

public:
    explicit Plot3DBase(QQuickItem *parent = 0);

    // Nutmegobject API
    Q_INVOKABLE void registerProperties(QMap<QString, QString> mapping);
    Q_INVOKABLE void registerProperties(QVariantMap mapping);
    Q_INVOKABLE QString mapProperty(const QString &prop);

    Axis3DBase* axis() const;

    QString handle() const;
    void setHandle(QString handle);

    QEntity *entity() const;

    QMatrix4x4 transform() const;
    void setTransform(QMatrix4x4 transform);

signals:
    void axisChanged(Axis3DBase* axis);
    void handleChanged(QString handle);
    void entityChanged(QEntity* entity);

    void transformChanged(QMatrix4x4 transform);

public slots:
    void updateAxis();

private:
    void setAxis(Axis3DBase* axis);
    Axis3DBase* m_axis;
    QString m_handle;
    QEntity* m_entity;
    Qt3D::QMatrixTransform *m_matrix;
    Qt3D::QTransform *m_transform;
};

#endif // PLOT3DBASE_H
