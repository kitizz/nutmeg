#include "pointcloud.h"

#include <Qt3DRenderer/QSphereMesh>
#include <Qt3DRenderer/QPhongMaterial>
#include <Qt3DCore/QTranslateTransform>

PointCloud::PointCloud(QQuickItem *parent)
    : Plot3DBase(parent)
    , m_xData(QList<qreal>())
    , m_yData(QList<qreal>())
    , m_zData(QList<qreal>())
    , m_dataLength(0)
    , m_entities(QList<Qt3D::QEntity*>)
    , m_points(QList<Qt3D::QTranslateTransform*>)
{

}

QList<qreal> PointCloud::xData() const
{
    return m_xData;
}

QList<qreal> PointCloud::yData() const
{
    return m_yData;
}

QList<qreal> PointCloud::zData() const
{
    return m_zData;
}

void PointCloud::setXData(QList<qreal> xData)
{
    if (m_xData == xData)
        return;

    m_xData = xData;
    emit xDataChanged(xData);
}

void PointCloud::setYData(QList<qreal> yData)
{
    if (m_yData == yData)
        return;

    m_yData = yData;
    emit yDataChanged(yData);
}

void PointCloud::setZData(QList<qreal> zData)
{
    if (m_zData == zData)
        return;

    m_zData = zData;
    emit zDataChanged(zData);
}

void PointCloud::updateData()
{
    int N = m_xData.length();
    if (m_yData.length() != N || m_zData.length() != N)
        return;

    // Create new entities as required
    while (m_dataLength < N) {
        Qt3D::QEntity *e = new Qt3D::QEntity();
        QSphereMesh *s = new QSphereMesh();
        s->setRadius(0.1);
        s->setRings(3);
        s->setSlices(3);

        QPhongMaterial *m = new QPhongMaterial();
        m->setDiffuse(QColor(qFabs(qCos(angle)) * 255, 204, 75));
        m->setAmbient(Qt::gray);
        m->setSpecular(Qt::white);
        m->setShininess(50.0f);

        QTranslateTransform *t = new QTranslateTransform();

        e->addComponent(s);
        e->addComponent(m);
        e->addComponent(t);

        m_points.append(t);
        m_entities.append(e);

        m_dataLength++;
    }

    while (m_dataLength > N) {
        auto e = m_entities.last();
        m_entities.removeLast();
        // Need to clean up all the created objects
        foreach (auto c, e->components()) {
            e->removeComponent(c);
            c->deleteLater();
        }
        delete e;
        // Point should have been deleted a few lines up
        m_points.removeLast();

        m_dataLength--;
    }

    for (int i = 0; i < m_dataLength; ++i) {
        auto p = m_points[i];
        p->setTranslation(QVector(m_xData[i], m_yData[i], m_zData[i]));
    }
}

