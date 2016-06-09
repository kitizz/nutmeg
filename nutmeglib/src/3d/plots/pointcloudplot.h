#ifdef SUPPORT_3D
#ifndef POINTCLOUD_H
#define POINTCLOUD_H

#include "plot3dbase.h"
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QAttribute>

#include "pointcloudgeometry.h"
#include "../../util/ndarray.h"

class NUTMEGLIB_EXPORT PointCloudPlot : public Plot3DBase
{
    Q_OBJECT
    Q_PROPERTY(NDArray vertices READ vertices WRITE setVertices NOTIFY verticesChanged)
    Q_PROPERTY(float pointsize READ pointsize WRITE setPointSize NOTIFY pointSizeChanged)

public:
    explicit PointCloudPlot(QQuickItem *parent = 0);

    NDArray vertices() const;
    void setVertices(NDArray xData);

    Qt3DRender::QMaterial *makeMaterial();

    // TODO:
//    Q_INVOKABLE void freezeUpdates();

    float pointsize() const;

public slots:
    void updateData();
    void setPointSize(float pointsize);

signals:
    void verticesChanged(NDArray xData);
    void pointSizeChanged(float pointsize);

private:
    NDArrayTyped<float> m_vertices;

    PointCloud *m_pointcloud;
    Qt3DRender::QAttribute *m_posAttr;
    Qt3DRender::QAttribute *m_normalAttr;

    int m_dataLength;
    QParameter *m_pointsizeParam;
};

#endif // POINTCLOUD_H
#endif
