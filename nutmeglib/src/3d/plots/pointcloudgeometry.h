#ifndef POINTCLOUDGEOMETRY_H
#define POINTCLOUDGEOMETRY_H

#include <Qt3DRender/QGeometry>
#include <Qt3DCore/QNode>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QBuffer>

#include "../../util/ndarray.h"

using namespace Qt3DRender;

class PointCloudGeometry : public Qt3DRender::QGeometry
{
    Q_OBJECT

public:
    explicit PointCloudGeometry(Qt3DCore::QNode *parent = Q_NULLPTR);
    ~PointCloudGeometry();

    void updateData(const NDArrayTyped<float> &vertices);

private:
    QBuffer *m_vertexBuffer;
    QByteArray m_data;
    QAttribute *m_posAttr;
    QAttribute *m_normalAttr;
};

class PointCloud : public Qt3DRender::QGeometryRenderer
{
    Q_OBJECT
public:
    explicit PointCloud(Qt3DCore::QNode *parent = 0);
    ~PointCloud();

    void updateData(const NDArrayTyped<float> &vertices);

private:
    PointCloudGeometry *m_geom;
};

#endif // POINTCLOUDGEOMETRY_H
