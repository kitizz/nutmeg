#ifndef RAYCLOUDPLOT_H
#define RAYCLOUDPLOT_H

#include <QQuickItem>

#include "plot3dbase.h"
#include "raycloudgeometry.h"
#include "../../util/ndarray.h"

class RayCloudPlot : public Plot3DBase
{
    Q_OBJECT
    Q_PROPERTY(NDArray start READ start WRITE setStart NOTIFY startChanged)
    Q_PROPERTY(NDArray end READ end WRITE setEnd NOTIFY endChanged)
    Q_PROPERTY(float linewidth READ linewidth WRITE setLinewidth NOTIFY linewidthChanged)

public:
    explicit RayCloudPlot(QQuickItem *parent=0);
    NDArray start() const;
    NDArray end() const;

    float linewidth() const;

public slots:
    void setStart(NDArray start);
    void setEnd(NDArray end);

    void updateData();
    void updatePixelSize();

    void setLinewidth(float linewidth);

signals:
    void startChanged(NDArray start);
    void endChanged(NDArray end);

    void linewidthChanged(float linewidth);

protected:
    QMaterial *makeMaterial();

private:
    NDArrayTyped<float> m_start;
    NDArrayTyped<float> m_end;
    RayCloud *m_raycloud;
    QParameter *m_linewidthParam;
    QParameter *m_pixelSize;
};

#endif // RAYCLOUDPLOT_H
