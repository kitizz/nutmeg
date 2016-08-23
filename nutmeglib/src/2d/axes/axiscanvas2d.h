#ifndef AXISCANVAS2D_H
#define AXISCANVAS2D_H

#include <QQuickFramebufferObject>

#include "axis2dbase.h"
#include "../../common/nutmegrenderer.h"

class Axis2DBase;
class Axis2DRenderer;
class AxisCanvas2D : public QQuickFramebufferObject::Renderer
{
public:
    AxisCanvas2D(Axis2DBase *axis);
    ~AxisCanvas2D();

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void render();

private:
    QSize m_size;
    Axis2DBase *m_axis;
    Axis2DRenderer *m_renderer;
};

#endif // AXISCANVAS2D_H
