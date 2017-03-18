#ifdef SUPPORT_3D
#ifndef RAYCLOUDGEOMETRY_H
#define RAYCLOUDGEOMETRY_H

#include <Qt3DRender/QGeometry>
#include <Qt3DCore/QNode>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QBuffer>
#include "../../util/ndarray.h"

using namespace Qt3DRender;

class RayCloudGeometry : public QGeometry
{
    Q_OBJECT

public:
    explicit RayCloudGeometry(Qt3DCore::QNode *parent = 0);
    ~RayCloudGeometry();

    bool updateData(const NDArrayTyped<float> &start, const NDArrayTyped<float> &end);

private:
    QBuffer *m_vertexBuffer;
    QBuffer *m_indexBuffer;
    QByteArray m_data;
    QByteArray m_index;
    QAttribute *m_posAttr;
    QAttribute *m_normalAttr;
    QAttribute *m_sideAttr;
    QAttribute *m_indexAttr;
};

class RayCloud : public QGeometryRenderer
{
    Q_OBJECT
public:
    explicit RayCloud(Qt3DCore::QNode *parent = 0);
    ~RayCloud();

    bool updateData(const NDArrayTyped<float> &start, const NDArrayTyped<float> &end);

private:
    RayCloudGeometry *m_geom;
};

#endif // RAYCLOUDGEOMETRY_H
#endif
