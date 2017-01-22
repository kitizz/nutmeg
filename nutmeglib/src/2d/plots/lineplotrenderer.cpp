#include "lineplotrenderer.h"
#include "lineplot.h"

//#include <QOpenGLContext>
//#include <QOpenGLFramebufferObjectFormat>

#include <QGuiApplication>
#include <QQuickWindow>

#include "../../util/util.h"
#include "../../util/ndarray.h"


LinePlotRenderer::LinePlotRenderer(LinePlot *plot)
    : NutmegRenderer()
    , m_vertexAttr(0)
    , m_matrixUnif(0)
    , m_pixelUnif(0)
    , m_vertexCount(0)
    , m_vertices(new float[1])
    , m_vertexBuffer(QOpenGLBuffer(QOpenGLBuffer::VertexBuffer))
    , m_vertexArray(new QOpenGLVertexArrayObject())
    , m_initialized(false)
    , m_plot(plot)
    , m_view(QMatrix3x3())
{
    m_view.setToIdentity();
}

LinePlotRenderer::~LinePlotRenderer()
{
    if (m_vertices)
        delete[] m_vertices;
    delete m_vertexArray;
}

void LinePlotRenderer::initialize()
{
    initializeOpenGLFunctions();

    QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, &program);
    vshader->compileSourceFile(":/shaders/lineshader.vert");

    QOpenGLShader *gshader = new QOpenGLShader(QOpenGLShader::Geometry, &program);
    gshader->compileSourceFile(":/shaders/lineshader.geom");

    QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, &program);
    fshader->compileSourceFile(":/shaders/lineshader.frag");

    program.addShader(vshader);
    program.addShader(gshader);
    program.addShader(fshader);
    program.link();

    m_vertexAttr = program.attributeLocation("myvertex");
    m_matrixUnif = program.uniformLocation("mymodelMatrix");
    m_pixelUnif = program.uniformLocation("pixel");

    m_initialized = true;
}

void LinePlotRenderer::render(QRectF view, QSize surfaceSize)
{
    QVector2D pixelSize(1.0/qreal(surfaceSize.width()), 1.0/qreal(surfaceSize.height()));

    updateGeometry();
    updateView();
//    qDebug() << "Rendering Line" << surfaceSize << m_vertexCount;

    QColor col = m_plot->line()->color();
    QVector4D lineColor(col.redF(), col.greenF(), col.blueF(), col.alphaF());
    qreal linewidth = m_plot->line()->width();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    program.bind();
    program.setUniformValue(m_pixelUnif, pixelSize);
    program.setUniformValue("thickness", float(linewidth*qApp->devicePixelRatio()));
    program.setUniformValue("lineColor", lineColor);
    program.setUniformValue("modelView", m_view);
    m_vertexArray->bind();
    glDrawArrays(GL_LINE_STRIP, 0, m_vertexCount);
    m_vertexArray->release();
    program.release();
}

void LinePlotRenderer::updateGeometry()
{
    if (!m_dirtyData || !m_plot) return;

    NDArray &xArray = m_plot->xData(),
            &yArray = m_plot->yData();

    bool emptyX = (m_plot->xSize() == 0);
    int N = (emptyX) ? m_plot->ySize() : qMin( m_plot->xSize(), m_plot->ySize() );

    if (yArray.type() == NDArray::Unknown)
        return; // Nothing to do

    // Create and bind the array if necessary
    if (!m_vertexArray->isCreated())
        m_vertexArray->create();
    m_vertexArray->bind();

    // Create and bind the buffer if necessary
    if (!m_vertexBuffer.isCreated()) {
        m_vertexBuffer.create();
        m_vertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    }
    m_vertexBuffer.bind();

    // Keep a track of the number of "primitives"
    m_vertexCount = N;
    int tuplesize = 2;
    int vsize = tuplesize*N;

    // Figure out if the vertex buffer needs to be expanded
    int bufsize = m_vertexBuffer.size();
    if (bufsize < vsize) {
        bufsize = ((vsize/1024) + 1) * 1024;
        // Reallocate CPU and GPu buffers
        delete[] m_vertices;
        m_vertices = new float[bufsize];
        m_vertexBuffer.allocate(bufsize*sizeof(float));
    }

    // Convert and copy values into the buffer
    if (emptyX) {
        // Default to index values
        for (int i = 0; i < N; ++i)
            m_vertices[tuplesize*i] = i;
    } else {
        xArray.convertTo<float>(m_vertices, 0, tuplesize);
    }
    yArray.convertTo<float>(m_vertices, 1, tuplesize);

    m_vertexBuffer.write(0, m_vertices, vsize*sizeof(float));

    program.enableAttributeArray(m_vertexAttr);
    program.setAttributeBuffer(m_vertexAttr, GL_FLOAT, 0, tuplesize);

    m_vertexBuffer.release();
    m_vertexArray->release();

    m_dirtyData = false;
}

void LinePlotRenderer::updateView()
{
    if (!m_plot) return;
    Axis2DBase *axis = m_plot->axis2d();
    if (!axis) return;

    // Scaled by 2 because surface bounds are [-1, 1]
    qreal sx = 2 / (axis->maxX() - axis->minX());
    qreal sy = 2 / (axis->maxY() - axis->minY());
    m_view(0, 0) = sx;
    m_view(1, 1) = sy;
    m_view(0, 3) = -0.5 * sx * (axis->maxX() + axis->minX());
    m_view(1, 3) = -0.5 * sy * (axis->maxY() + axis->minY());
}
