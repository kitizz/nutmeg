#include "plot2dbase.h"
#include "axisbase.h"
#include <QPolygonF>
#include <QPainter>

#include <algorithm>

#include "../QKDTree/QVectorND/QVectorND.h"

Plot2DBase::Plot2DBase(QQuickItem *parent)
    : QQuickItem(parent)
    , m_axis(0)
    , m_handle(QString())
    , m_canvas(0)
    , m_dataLimits(QRectF())
{
    connect(this, &QQuickItem::parentChanged, this, &Plot2DBase::updateAxis);
}

AxisBase *Plot2DBase::axis() const
{
    return m_axis;
}

void Plot2DBase::print(QPainter *painter)
{
    if (!m_canvas)
        return;
    m_canvas->paint(painter);
}

QString Plot2DBase::handle() const
{
    return m_handle;
}

void Plot2DBase::setHandle(QString arg)
{
    if (m_handle == arg) return;
    m_handle = arg;
    emit handleChanged(arg);
}

QQuickPaintedItem *Plot2DBase::canvas() const
{
    return m_canvas;
}

void Plot2DBase::setCanvas(QQuickPaintedItem *arg)
{
    if (m_canvas == arg) return;
    m_canvas = arg;
    emit canvasChanged(arg);
}

QPointF Plot2DBase::itemToData(QPointF point)
{
    if (!m_axis) return point;
    qreal px = point.x(), py = height() - point.y();

    QRectF lim = m_axis->limits();
    qreal sx = lim.width() / width();
    qreal sy = lim.height() / height();
    if (width() == 0 || height() == 0) {
        qWarning() << "Zero width or height" << width() << height();
    }
    return QPointF(sx*px + lim.left(), sy*py + lim.top());
}

QList<QPointF> Plot2DBase::itemToDataList(QList<QPointF> points)
{
    // TODO: Research using Armadillo to do transformations...
    if (!m_axis) return points;

    QList<QPointF> newPoints;
    foreach (QPointF p, points) {
        qreal x = p.x(), y = height() - p.y();

        QRectF lim = m_axis->limits();
        qreal sx = lim.width() / width();
        qreal sy = lim.height() / height();

        newPoints << QPointF(sx*x + lim.left(), sy*y + lim.top());
    }
    return newPoints;
}

QPointF Plot2DBase::dataToItem(QPointF point)
{
    if (!m_axis) return point;
    qreal x = point.x(), y = point.y();

    QRectF lim = m_axis->limits();
    qreal sx = 1;
    qreal sy = 1;
    if (lim.width() != 0)
        sx = width() / lim.width();
    if (lim.height() != 0)
        sy = height() / lim.height();

    return QPointF( (x - lim.left())*sx, height() - (y - lim.top())*sy );
}

QList<QPointF> Plot2DBase::dataToItemList(QList<QPointF> points)
{
    // TODO: Research using Armadillo to do transformations...
    if (!m_axis) return points;

    QList<QPointF> newPoints;
    foreach (QPointF p, points) {
        qreal x = p.x(), y = p.y();

        QRectF lim = m_axis->limits();
        qreal sx = width() / lim.width();
        qreal sy = height() / lim.height();

        newPoints << QPointF( (x - lim.left())*sx, height() - (y - lim.top())*sy );
    }
    return newPoints;
}

QRectF Plot2DBase::dataLimits() const
{
    return m_dataLimits;
}

void Plot2DBase::registerProperties(QMap<QString, QString> mapping)
{
    NutmegObject::registerProperties(mapping);
}

void Plot2DBase::registerProperties(QVariantMap mapping)
{
    QMap<QString, QString> map;
    foreach (QString tag, mapping.keys()) {
        QString prop = mapping.value(tag).toString();
        if (!prop.isEmpty())
            map.insert(tag, prop);
    }
    registerProperties(map);
}

QString Plot2DBase::mapProperty(const QString &prop)
{
    return NutmegObject::mapProperty(prop);
}

void Plot2DBase::setAxis(AxisBase *arg)
{
    if (m_axis == arg) return;

    if (m_axis) {
        m_axis->disconnect(this);
        m_axis->deregisterPlot(this);
    }

    m_axis = arg;

    if (m_axis) {
        connect(m_axis, &AxisBase::minXChanged, this, &Plot2DBase::triggerUpdate);
        connect(m_axis, &AxisBase::minYChanged, this, &Plot2DBase::triggerUpdate);
        connect(m_axis, &AxisBase::maxXChanged, this, &Plot2DBase::triggerUpdate);
        connect(m_axis, &AxisBase::maxYChanged, this, &Plot2DBase::triggerUpdate);
        connect(m_axis, &AxisBase::dataLimitsChanged, this, &Plot2DBase::triggerUpdate);
        connect(m_axis, &AxisBase::xAxisChanged, this, &Plot2DBase::triggerUpdate);
        connect(m_axis, &AxisBase::yAxisChanged, this, &Plot2DBase::triggerUpdate);

        m_axis->registerPlot(this);

    } else {
        qWarning() << Q_FUNC_INFO << "Plot is not a descendant of any Axis. It may not behave as expected.";
    }
    emit axisChanged(arg);
}

void Plot2DBase::triggerUpdate()
{
//    update();
    if (m_canvas)
        m_canvas->update();
}

void Plot2DBase::updateAxis()
{
    // Work up the tree until the next Axis item is found.
    QObject *newParent = parent();
    AxisBase *axis;
    while (newParent) {
        axis = qobject_cast<AxisBase*>(newParent);
        if (axis) break;
        newParent = newParent->parent();
    }

    setAxis(axis);
}

void Plot2DBase::setDataLimits(QRectF arg)
{
    if (m_dataLimits == arg) return;
    m_dataLimits = arg;
    emit dataLimitsChanged(arg);
    triggerUpdate();
}
