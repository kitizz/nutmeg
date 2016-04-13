//#ifdef SUPPORT_3D
#include "pointcloud.h"

#include <Qt3DRender/QSphereMesh>
#include <Qt3DRender/QPhongMaterial>
#include <Qt3DCore/QTransform>
#include <QtMath>

PointCloud::PointCloud(QQuickItem *parent)
    : Plot3DBase(parent)
    , m_xData(QList<qreal>())
    , m_yData(QList<qreal>())
    , m_zData(QList<qreal>())
    , m_dataLength(0)
    , m_entities(QList<Qt3DCore::QEntity*>())
    , m_points(QList<Qt3DCore::QTransform*>())
{
    QMap<QString,QString> props;
    props.insert("x", "xData");
    props.insert("y", "yData");
    props.insert("z", "zData");
    registerProperties(props);

    connect(this, &PointCloud::xDataChanged, this, &PointCloud::updateData);
    connect(this, &PointCloud::yDataChanged, this, &PointCloud::updateData);
    connect(this, &PointCloud::zDataChanged, this, &PointCloud::updateData);
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
    return;
    int N = m_xData.length();
    if (m_yData.length() != N || m_zData.length() != N)
        return;

    // Create new entities as required
    while (m_dataLength < N) {
        Qt3DCore::QEntity *e = new Qt3DCore::QEntity(entity());
        Qt3DRender::QSphereMesh *s = new Qt3DRender::QSphereMesh();
        s->setRadius(3);
        s->setRings(10);
        s->setSlices(10);

        Qt3DRender::QPhongMaterial *m = new Qt3DRender::QPhongMaterial();
        m->setDiffuse(QColor(qFabs(qCos(0.8)) * 255, 204, 75));
        m->setAmbient(Qt::gray);
        m->setSpecular(Qt::white);
        m->setShininess(150.0f);

        Qt3DCore::QTransform *trans = new Qt3DCore::QTransform();

        e->addComponent(s);
        e->addComponent(m);
        e->addComponent(trans);

        m_points.append(trans);
        m_entities.append(e);

        e->setParent(entity());

        m_dataLength++;
        qDebug() << "Adding point entity:" << e;
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
        p->setTranslation(QVector3D(m_xData[i], m_yData[i], m_zData[i]));
    }
    qDebug() << "Points:" << m_points;
}

//#endif
