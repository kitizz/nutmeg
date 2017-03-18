#ifdef SUPPORT_3D
#include "pointcloudgeometry.h"

PointCloudGeometry::PointCloudGeometry(Qt3DCore::QNode *parent)
    : Qt3DRender::QGeometry(parent)
    , m_vertexBuffer(new QBuffer(QBuffer::VertexBuffer, this))
    , m_data(QByteArray())
    , m_posAttr(new Qt3DRender::QAttribute(this))
    , m_normalAttr(new Qt3DRender::QAttribute(this))
{
    addAttribute(m_posAttr);
    addAttribute(m_normalAttr);
}

PointCloudGeometry::~PointCloudGeometry()
{
    delete m_posAttr;
    delete m_normalAttr;
    delete m_vertexBuffer;
}

void PointCloudGeometry::updateData(const NDArrayTyped<float> &vertices)
{
    int N = vertices.shape(0);

    const qint32 elSize = 6;  // vx vy vz nx ny nz
    const qint32 stride = elSize * sizeof(float);

    int size = stride * N;
    m_data.resize(size);

    float *data = reinterpret_cast<float*>(m_data.data());
    float *ptr = data - 1;
    auto srcptr = vertices.cbegin(); --srcptr;
    int n = N;
    int i = -1;  // Performance on avoiding "i++"
    while (n--) {
        *(++ptr) = *(++srcptr);
        *(++ptr) = *(++srcptr);
        *(++ptr) = *(++srcptr);
        *(++ptr) = 1;
        *(++ptr) = 0;
        *(++ptr) = 0;
    }

    m_vertexBuffer->setData(m_data);

    m_posAttr->setName(QAttribute::defaultPositionAttributeName());
    m_posAttr->setDataType(QAttribute::Float);
    m_posAttr->setDataSize(3);
    m_posAttr->setAttributeType(QAttribute::VertexAttribute);
    m_posAttr->setBuffer(m_vertexBuffer);
    m_posAttr->setByteStride(stride);
//    m_posAttr->setByteOffset(3 * sizeof(float));
    m_posAttr->setCount(N);

    m_normalAttr->setName(QAttribute::defaultNormalAttributeName());
    m_normalAttr->setDataType(QAttribute::Float);
    m_normalAttr->setDataSize(3);
    m_normalAttr->setAttributeType(QAttribute::VertexAttribute);
    m_normalAttr->setBuffer(m_vertexBuffer);
    m_normalAttr->setByteStride(stride);
    m_normalAttr->setByteOffset(3 * sizeof(float));
    m_normalAttr->setCount(N);
}


// -----------------------------------------------
//          PointCloud
// -----------------------------------------------
PointCloud::PointCloud(Qt3DCore::QNode *parent)
    : QGeometryRenderer(parent)
    , m_geom(new PointCloudGeometry(this))
{
    setPrimitiveType(QGeometryRenderer::Points);
    setGeometry(m_geom);
}

PointCloud::~PointCloud()
{
    QNode::cleanup();
}

void PointCloud::updateData(const NDArrayTyped<float> &vertices)
{
    m_geom->updateData(vertices);
}

#endif
