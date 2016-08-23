#ifdef SUPPORT_3D
#include "raycloudplot.h"

#include "../../util/arrayutil.h"
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QGraphicsApiFilter>
//#include <Qt3DRender/QAnnotation>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QPointSize>
#include <Qt3DRender/QCullFace>
#include <QVector2D>


RayCloudPlot::RayCloudPlot(QQuickItem *parent)
    : Plot3DBase(parent)
    , m_start(NDArrayTyped<float>({0}))
    , m_end(NDArrayTyped<float>({0}))
    , m_raycloud(new RayCloud())
    , m_linewidthParam(new QParameter("linewidth", 10.0f))
    , m_pixelSize(new QParameter("pixelSize", QVector2D(1.0, 1.0)))
{
    QMap<QString,QString> props;
    props.insert("start", "start");
    props.insert("end", "end");
    registerProperties(props);

//    entity()->addComponent(m_raycloud);
    entity()->addComponent(makeMaterial());

    connect(this, &QQuickItem::widthChanged, this, &RayCloudPlot::updatePixelSize);
    connect(this, &QQuickItem::heightChanged, this, &RayCloudPlot::updatePixelSize);
    updatePixelSize();
}

NDArray RayCloudPlot::start() const
{
    return m_start;
}

NDArray RayCloudPlot::end() const
{
    return m_end;
}

float RayCloudPlot::linewidth() const
{
    return m_linewidthParam->value().toFloat();
}

void RayCloudPlot::setLinewidth(float linewidth)
{
    float val = m_linewidthParam->value().toFloat();
    if (val == linewidth)
        return;

    m_linewidthParam->setValue(linewidth);
    emit linewidthChanged(linewidth);
}

void RayCloudPlot::setStart(NDArray start)
{
    bool success;
    auto conv = make3d(start, success);
    if (!success)
        return;

    m_start = conv;
    updateData();
    emit startChanged(start);
}

void RayCloudPlot::setEnd(NDArray end)
{
    bool success;
    auto conv = make3d(end, success);
    if (!success)
        return;

    m_end = conv;
    updateData();
    emit endChanged(end);
}

QMaterial *RayCloudPlot::makeMaterial()
{
    auto mat = new QMaterial();
    auto eff = new QEffect(mat);

    mat->addParameter(m_linewidthParam);
    mat->addParameter(m_pixelSize);
    mat->addParameter(m_colorParam);

    // Set up the shader
    auto gl3tech = new QTechnique();
    auto api = gl3tech->graphicsApiFilter();
    api->setApi(QGraphicsApiFilter::OpenGL);
//    api->setMajorVersion(3);
//    api->setMinorVersion(1);
//    api->setProfile(QGraphicsApiFilter::CoreProfile);
    api->setMajorVersion(2);
    api->setMinorVersion(0);
    api->setProfile(QGraphicsApiFilter::NoProfile);

    // Forward render
    auto ann = new QAnnotation();
    ann->setName("renderingStyle");
    ann->setValue("forward");

    // Shader render pass
    auto gl3program = new QShaderProgram();
    gl3program->setVertexShaderCode(QShaderProgram::loadSource(QUrl("qrc:/shaders/ray.vert")));
    gl3program->setFragmentShaderCode(QShaderProgram::loadSource(QUrl("qrc:/shaders/ray.frag")));

    auto gl3pass = new QRenderPass();
    gl3pass->setShaderProgram(gl3program);

//    auto pointsize = new QPointSize();
//    pointsize->setEnabled(true);
//    gl3pass->addRenderState(pointsize);

//    auto cullface = new QCullFace();
//    cullface->setMode(QCullFace::Front);
//    cullface->setEnabled(true);
//    gl3pass->addRenderState(cullface);

    gl3tech->addAnnotation(ann);
    gl3tech->addPass(gl3pass);

    // Push it all in
    eff->addTechnique(gl3tech);
    mat->setEffect(eff);
    return mat;
}

void RayCloudPlot::updateData()
{
    bool notempty = m_raycloud->updateData(m_start, m_end);
//    entity()->setEnabled(notempty);

    if (!notempty) {
        entity()->removeComponent(m_raycloud);
        return;
    }

    qDebug() << "\tAdding raycloud object";
    entity()->addComponent(m_raycloud);

    // Update ranges
    // TODO: Consider making this lazy. I.e. whenever requested to scale the plot to datalimits...
    QList<RangeValues> startlim = ArrayUtil::limitsAxis(m_start, 0);
    QList<RangeValues> endlim = ArrayUtil::limitsAxis(m_end, 0);

    QList<qreal> lims;
    for (int i=0; i<3; ++i) {
        lims << qMin(startlim[i].min, endlim[i].min);
        lims << qMax(startlim[i].max, endlim[i].max);
    }
    setDataLimits(lims);

//    qDebug() << "Update data..";
    //    axis3d()->dumpTree(entity()->parentNode(), 0);
}

void RayCloudPlot::updatePixelSize()
{
    m_pixelSize->setValue(QVector2D(1/width(), 1/height()));
}

#endif
