#ifdef SUPPORT_3D
#include "plot3dbase.h"

#include "../../util/util.h"

#include <QGeometryRenderer>
//#include <Qt3DRender/QM>

Plot3DBase::Plot3DBase(QQuickItem *parent)
    : PlotBase(parent)
    , m_colorParam(new Qt3DRender::QParameter("plotColor", QVector4D(0.0,0.0,0.0,1.0)))
    , m_axis(0)
    , m_entity(new QEntity())
    , m_transform(new Qt3DCore::QTransform())
    , m_dataLimits(QList<qreal>())
{
    QMap<QString, QString> props;
    props.insert("color", "color");
    registerProperties(props);

    // TODO: Make a 3D Limits class...
    m_dataLimits << Inf << -Inf << Inf << -Inf << Inf << -Inf;

    m_entity->addComponent(m_transform);
}

Axis3DBase *Plot3DBase::axis3d() const
{
    return m_axis;
}

QEntity *Plot3DBase::entity() const
{
    return m_entity;
}

QMatrix4x4 Plot3DBase::transform() const
{
    return m_transform->matrix();
}

void Plot3DBase::setTransform(QMatrix4x4 transform)
{
    if (m_transform->matrix() == transform)
        return;

    m_transform->setMatrix(transform);
    emit transformChanged(transform);
}

QList<qreal> Plot3DBase::dataLimits() const
{
    return m_dataLimits;
}

QColor Plot3DBase::color() const
{
    QVector4D current = qvariant_cast<QVector4D>(m_colorParam->value());
    QColor col;
    col.setRgbF(current.x(), current.y(), current.z(), current.w());
    return col;
}

void Plot3DBase::setColor(QColor color)
{
    qDebug() << "\t\tSetting color:" << color;
    QVector4D current = qvariant_cast<QVector4D>(m_colorParam->value());
    QVector4D col(color.redF(), color.greenF(), color.blueF(), color.alphaF());
    if (col == current)
        return;

    m_colorParam->setValue(col);
    emit colorChanged(color);
}

void Plot3DBase::updateAxis(AxisBase *oldAxis, AxisBase *newAxis)
{
    auto old3DAxis = qobject_cast<Axis3DBase*>(oldAxis);
    if (old3DAxis) {
        old3DAxis->disconnect(this);
        old3DAxis->deregister3DPlot(this);
    }

    m_axis = qobject_cast<Axis3DBase*>(newAxis);
    if (m_axis) {
        m_axis->register3DPlot(this);
        connect(this, &Plot3DBase::dataLimitsChanged, m_axis, &Axis3DBase::updateDataLimits);

    } else {
        qWarning() << Q_FUNC_INFO << "Plot is not a descendant of any Axis. It may not behave as expected.";
    }
}

void Plot3DBase::addComponent(QComponent *comp)
{
    m_entity->addComponent(comp);
}

void Plot3DBase::updateDataLimits()
{
}

void Plot3DBase::setDataLimits(QList<qreal> dataLimits)
{
    if (m_dataLimits == dataLimits)
        return;

    m_dataLimits = dataLimits;
    emit dataLimitsChanged(dataLimits);
}

NDArrayTyped<float> Plot3DBase::make3d(NDArray array, bool &success)
{
    int A = array.size();
    if (A % 3 != 0) {
        qWarning() << "WARNING: Bad data size, must be multiple of 3";
        success = false;
        return NDArrayTyped<float>({0});
    }
    NDArrayTyped<float> vertices = array.convert<float>();
    QList<int> shp;
    shp << A/3 << 3;
    vertices.setShape(shp);
    success = true;

    return vertices;
}

#endif
