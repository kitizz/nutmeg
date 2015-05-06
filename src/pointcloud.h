#ifdef SUPPORT_3D
#ifndef POINTCLOUD_H
#define POINTCLOUD_H

#include "plot3dbase.h"
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTranslateTransform>

class PointCloud : public Plot3DBase
{
    Q_OBJECT
    Q_PROPERTY(QList<qreal> xData READ xData WRITE setXData NOTIFY xDataChanged)
    Q_PROPERTY(QList<qreal> yData READ yData WRITE setYData NOTIFY yDataChanged)
    Q_PROPERTY(QList<qreal> zData READ zData WRITE setZData NOTIFY zDataChanged)

public:
    explicit PointCloud(QQuickItem *parent = 0);

    QList<qreal> xData() const;
    void setXData(QList<qreal> xData);

    QList<qreal> yData() const;
    void setYData(QList<qreal> yData);

    QList<qreal> zData() const;
    void setZData(QList<qreal> zData);

    // TODO:
//    Q_INVOKABLE void freezeUpdates();

public slots:
    void updateData();
signals:
    void xDataChanged(QList<qreal> xData);
    void yDataChanged(QList<qreal> yData);
    void zDataChanged(QList<qreal> zData);
private:
    QList<qreal> m_xData;
    QList<qreal> m_yData;
    QList<qreal> m_zData;
    int m_dataLength;
    QList<Qt3D::QEntity*> m_entities;
    QList<Qt3D::QTranslateTransform*> m_points;
};

#endif // POINTCLOUD_H
#endif
