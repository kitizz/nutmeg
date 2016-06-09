#include "raycloudgeometry.h"

#include <QtMath>

RayCloudGeometry::RayCloudGeometry(Qt3DCore::QNode *parent)
    : QGeometry(parent)
    , m_vertexBuffer(new QBuffer(QBuffer::VertexBuffer, this))
    , m_indexBuffer(new QBuffer(QBuffer::IndexBuffer, this))
    , m_data(QByteArray())
    , m_index(QByteArray())
    , m_posAttr(new QAttribute(this))
    , m_normalAttr(new QAttribute(this))
    , m_sideAttr(new QAttribute(this))
    , m_indexAttr(new QAttribute(this))
{
    m_posAttr->setCount(0);
    m_normalAttr->setCount(0);
    m_sideAttr->setCount(0);
    m_indexAttr->setCount(0);

    addAttribute(m_posAttr);
    addAttribute(m_normalAttr);
    addAttribute(m_sideAttr);
    addAttribute(m_indexAttr);
}

RayCloudGeometry::~RayCloudGeometry() {
    delete m_posAttr;
    delete m_normalAttr;
    delete m_indexAttr;
    delete m_vertexBuffer;
    delete m_indexBuffer;
}

bool RayCloudGeometry::updateData(const NDArrayTyped<float> &start, const NDArrayTyped<float> &end) {
    int N = start.shape(0);
    if (end.shape(0) != N)
        return false;

    if (N == 0) {
        qDebug() << "Empty RayCloud Geometry";
        m_posAttr->setCount(0);
        m_normalAttr->setCount(0);
        m_sideAttr->setCount(0);
        m_indexAttr->setCount(0);
    }

    const qint32 elSize = 7;  // vx vy vz nx ny nz side
    const qint32 stride = elSize * sizeof(float);

    int size = stride * 4 * N;
    m_data.resize(size);
    int indexSize = 2 * 3 * N;  // 2 tris per lines, 3 verts per tri, N lines
    m_index.resize(indexSize * sizeof(quint16));

    float *data = reinterpret_cast<float*>(m_data.data());

    // Performance by starting at -1 and avoiding "d++"
    float *ptr = data - 1;
    const float *sd = start.cbegin(); --sd;
    const float *ed = end.cbegin(); --ed;
    int n = N;

    float x1, y1, z1;
    float x2, y2, z2;
    float nx, ny, nz;
    while (n--) {
        x1 = *(++sd);
        y1 = *(++sd);
        z1 = *(++sd);
        x2 = *(++ed);
        y2 = *(++ed);
        z2 = *(++ed);
        // Tangent
        nx = x2 - x1;
        ny = y2 - y1;
        nz = z2 - z1;
        float mag = sqrtf(nx*nx + ny*ny + nz*nz);
        if (mag != 0)
            mag = 1/mag;
        nx *= mag;
        ny *= mag;
        nz *= mag;

        // Side A, Start
        *(++ptr) = x1;
        *(++ptr) = y1;
        *(++ptr) = z1;
        *(++ptr) = nx;
        *(++ptr) = ny;
        *(++ptr) = nz;
        *(++ptr) = 1.0;
        // Side B, Start
        *(++ptr) = x1;
        *(++ptr) = y1;
        *(++ptr) = z1;
        *(++ptr) = -nx;
        *(++ptr) = -ny;
        *(++ptr) = -nz;
        *(++ptr) = -1.0;

        // Side A, End
        *(++ptr) = x2;
        *(++ptr) = y2;
        *(++ptr) = z2;
        *(++ptr) = nx;
        *(++ptr) = ny;
        *(++ptr) = nz;
        *(++ptr) = 1.0;
        // Side B, End
        *(++ptr) = x2;
        *(++ptr) = y2;
        *(++ptr) = z2;
        *(++ptr) = -nx;
        *(++ptr) = -ny;
        *(++ptr) = -nz;
        *(++ptr) = -1.0;
    }
    m_vertexBuffer->setData(m_data);

    // Sort out the indices
    quint16 *indices = reinterpret_cast<quint16*>(m_index.data());
    quint16 *indptr = indices - 1;
    n = N;
    quint16 i = 0;
    while (n--) {
        *(++indptr) = i;
        *(++indptr) = i + 1;
        *(++indptr) = i + 2;
        *(++indptr) = i + 2;
        *(++indptr) = i + 1;
        *(++indptr) = i + 3;
        i += 4;
    }
    m_indexBuffer->setData(m_index);

    // Organize the vertex attributes
    m_posAttr->setName(QAttribute::defaultPositionAttributeName());
    m_posAttr->setDataType(QAttribute::Float);
    m_posAttr->setDataSize(3);
    m_posAttr->setAttributeType(QAttribute::VertexAttribute);
    m_posAttr->setBuffer(m_vertexBuffer);
    m_posAttr->setByteStride(stride);
    m_posAttr->setCount(4*N);

    m_normalAttr->setName("vertexNormal");
    m_normalAttr->setDataType(QAttribute::Float);
    m_normalAttr->setDataSize(3);
    m_normalAttr->setAttributeType(QAttribute::VertexAttribute);
    m_normalAttr->setBuffer(m_vertexBuffer);
    m_normalAttr->setByteStride(stride);
    m_normalAttr->setByteOffset(3 * sizeof(float));
    m_normalAttr->setCount(4*N);

    m_sideAttr->setName("vertexSide");
    m_sideAttr->setDataType(QAttribute::Float);
    m_sideAttr->setDataSize(1);
    m_sideAttr->setAttributeType(QAttribute::VertexAttribute);
    m_sideAttr->setBuffer(m_vertexBuffer);
    m_sideAttr->setByteStride(stride);
    m_sideAttr->setByteOffset(6 * sizeof(float));
    m_sideAttr->setCount(4*N);

    // The index attribute
    m_indexAttr->setAttributeType(QAttribute::IndexAttribute);
    m_indexAttr->setDataType(QAttribute::UnsignedShort);
    m_indexAttr->setBuffer(m_indexBuffer);
    m_indexAttr->setCount(indexSize);

    return true;
}

RayCloud::RayCloud(Qt3DCore::QNode *parent)
    : QGeometryRenderer(parent)
    , m_geom(new RayCloudGeometry(this))
{
    setPrimitiveType(QGeometryRenderer::Triangles);
    setGeometry(m_geom);
}

RayCloud::~RayCloud()
{

}

bool RayCloud::updateData(const NDArrayTyped<float> &start, const NDArrayTyped<float> &end)
{
    return m_geom->updateData(start, end);
}
