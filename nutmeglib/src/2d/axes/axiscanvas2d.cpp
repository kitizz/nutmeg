#include "axiscanvas2d.h"

#include <QOpenGLFramebufferObjectFormat>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QQuickWindow>

//#include <QOpenGLFunctions_3_3_Core>
//#include <QOpenGLShaderProgram>
//#include <QOpenGLVertexArrayObject>
//#include <QOpenGLBuffer>

#include "axis2drenderer.h"

AxisCanvas2D::AxisCanvas2D(Axis2DBase *axis)
    : QQuickFramebufferObject::Renderer()
    , m_size(QSize())
    , m_axis(axis)
    , m_renderer(new Axis2DRenderer())
{
    m_renderer->initialize();
}

AxisCanvas2D::~AxisCanvas2D()
{
    delete m_renderer;
}

QOpenGLFramebufferObject *AxisCanvas2D::createFramebufferObject(const QSize &size)
{
    qDebug() << "Create FrameBufferObject for Axis:" << size;
    m_size = size;
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    // optionally enable multisampling by doing format.setSamples(4);
    format.setSamples(1);
    return new QOpenGLFramebufferObject(size, format);
}

void AxisCanvas2D::render()
{
    // Update the view bounds to match the axis
    QRectF view; // TODO

    // Let's go
    auto context = QOpenGLContext::currentContext();
    QOpenGLFunctions *f = context->functions();
    f->glClearColor(0, 0, 0, 0);
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_renderer->render(view, m_size);

    foreach (PlotBase* plot, m_axis->getPlotList()) {
        NutmegRenderer *pltRenderer = plot->renderer();
        if (pltRenderer) {
            pltRenderer->render(view, m_size);
        }
    }

    // Gotta reset the OpenGL state for QML
    m_axis->window()->resetOpenGLState();
}

void AxisCanvas2D::doLayout()
{
    // TODO: This is where the layout and plotRect should be estimated
    // axis.setPreferredPlotRect(...)
    // Refer to axisCanvas2D in master
}
