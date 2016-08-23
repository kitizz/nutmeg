#ifndef LINEPLOTCANVAS_H
#define LINEPLOTCANVAS_H

#include "plotcanvas.h"
#include "../../common/nutmegrenderer.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

class LinePlot;
class LinePlotRenderer : public NutmegRenderer
{
public:
    LinePlotRenderer(LinePlot* plot);
    ~LinePlotRenderer();

    virtual void initialize();
    void updateGeometry();
    void updateView();
    virtual void render(QRectF view, QSize surfaceSize);

private:
    QOpenGLShaderProgram program;

    int m_vertexAttr;
    int m_matrixUnif;
    int m_pixelUnif;
    int m_vertexCount;
    float *m_vertices;
    QOpenGLBuffer m_vertexBuffer;
    QOpenGLVertexArrayObject *m_vertexArray;
    bool m_initialized;
    LinePlot *m_plot;
    QMatrix4x4 m_view;
};

#endif // LINEPLOTCANVAS_H
