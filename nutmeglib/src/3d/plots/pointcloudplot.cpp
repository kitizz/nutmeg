//#ifdef SUPPORT_3D
#include "pointcloudplot.h"

#include <Qt3DRender/QEffect>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QGraphicsApiFilter>
#include <Qt3DRender/QAnnotation>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QPointSize>
#include <Qt3DRender/QParameter>

#include "../../util/arrayutil.h"

#include <Qt3DCore/QTransform>
#include <QtMath>

using namespace Qt3DRender;

PointCloudPlot::PointCloudPlot(QQuickItem *parent)
    : Plot3DBase(parent)
    , m_vertices(NDArrayTyped<float>({0}))
    , m_pointcloud(new PointCloud())
    , m_dataLength(0)
    , m_pointsizeParam(new QParameter("pointSize", 5.0f))
{
    QMap<QString,QString> props;
    props.insert("vertices", "vertices");
    props.insert("pointSize", "pointSize");
    registerProperties(props);

    entity()->addComponent(m_pointcloud);
    entity()->addComponent(makeMaterial());
}

NDArray PointCloudPlot::vertices() const
{
    return m_vertices;
}

void PointCloudPlot::setVertices(NDArray data)
{
    bool success;
    auto conv = make3d(data, success);
    if (!success)
        return;
    m_vertices = conv;

    updateData();
    emit verticesChanged(m_vertices);
}

QMaterial *PointCloudPlot::makeMaterial()
{
    auto mat = new QMaterial();
    auto eff = new QEffect(mat);

    // Add the point size parameter
    mat->addParameter(m_pointsizeParam);
    mat->addParameter(m_colorParam);

    // Set up the shader
    auto gl3tech = new QTechnique();
    auto api = gl3tech->graphicsApiFilter();
    api->setApi(QGraphicsApiFilter::OpenGL);
    api->setMajorVersion(2);
    api->setMinorVersion(0);
    api->setProfile(QGraphicsApiFilter::NoProfile);

    // Forward render
    auto ann = new QAnnotation();
    ann->setName("renderingStyle");
    ann->setValue("forward");

    // Shader render pass
    auto gl3program = new QShaderProgram();
    gl3program->setVertexShaderCode(QShaderProgram::loadSource(QUrl("qrc:/shaders/point.vert")));
    gl3program->setFragmentShaderCode(QShaderProgram::loadSource(QUrl("qrc:/shaders/point.frag")));

    auto gl3pass = new QRenderPass();
    gl3pass->setShaderProgram(gl3program);
    auto size = new QPointSize();
    size->setSpecification(QPointSize::StaticValue);
    size->setValue(pointsize());
    size->setEnabled(true);
    gl3pass->addRenderState(size);

    gl3tech->addAnnotation(ann);
    gl3tech->addPass(gl3pass);

    // Push it all in
    eff->addTechnique(gl3tech);
    mat->setEffect(eff);
    return mat;
}

float PointCloudPlot::pointsize() const
{
    return m_pointsizeParam->value().toFloat();
}

void PointCloudPlot::updateData()
{
    m_pointcloud->updateData(m_vertices);

    // Update ranges
    // TODO: Consider making this lazy. I.e. whenever requested to scale the plot to datalimits...
    QList<RangeValues> limlist = ArrayUtil::limitsAxis(m_vertices, 0);

    QList<qreal> lims;
    for (int i=0; i<3; ++i) {
        lims << limlist[i].min << limlist[i].max;
    }
    setDataLimits(lims);
}

void PointCloudPlot::setPointSize(float pointSize)
{
    float val = m_pointsizeParam->value().toFloat();
    if (val == pointSize)
        return;

    m_pointsizeParam->setValue(pointSize);
    emit pointSizeChanged(pointSize);
}

//#endif
