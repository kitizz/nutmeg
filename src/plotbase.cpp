#include "plotbase.h"
#include "axisbase.h"
#include <QPolygonF>
#include <QPainter>

#include <algorithm>

#include "../QKDTree/QVectorND/QVectorND.h"

PlotBase::PlotBase(QQuickItem *parent)
    : QQuickItem(parent)
    , m_axis(0)
    , m_handle(QString())
    , m_canvas(0)
    , m_dataLimits(QRectF())
{
    connect(this, &QQuickItem::parentChanged, this, &PlotBase::updateAxis);
}

AxisBase *PlotBase::axis() const
{
    return m_axis;
}

QString PlotBase::handle() const
{
    return m_handle;
}

void PlotBase::setHandle(QString arg)
{
    if (m_handle == arg) return;
    m_handle = arg;
    emit handleChanged(arg);
}

QQuickPaintedItem *PlotBase::canvas() const
{
    return m_canvas;
}

void PlotBase::setCanvas(QQuickPaintedItem *arg)
{
    if (m_canvas == arg) return;
    m_canvas = arg;
    emit canvasChanged(arg);
}

QString PlotBase::map(QString prop)
{
    return NutmegObject::map(prop);
}

QPointF PlotBase::itemToData(QPointF point)
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

QList<QPointF> PlotBase::itemToDataList(QList<QPointF> points)
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

QPointF PlotBase::dataToItem(QPointF point)
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

QList<QPointF> PlotBase::dataToItemList(QList<QPointF> points)
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

QRectF PlotBase::dataLimits() const
{
    return m_dataLimits;
}

void PlotBase::setAxis(AxisBase *arg)
{
    if (m_axis == arg) return;

    if (m_axis) {
        m_axis->disconnect(this);
        m_axis->deregisterPlot(this);
    }

    m_axis = arg;

    if (m_axis) {
        connect(m_axis, &AxisBase::minXChanged, this, &PlotBase::triggerUpdate);
        connect(m_axis, &AxisBase::minYChanged, this, &PlotBase::triggerUpdate);
        connect(m_axis, &AxisBase::maxXChanged, this, &PlotBase::triggerUpdate);
        connect(m_axis, &AxisBase::maxYChanged, this, &PlotBase::triggerUpdate);
        connect(m_axis, &AxisBase::dataLimitsChanged, this, &PlotBase::triggerUpdate);
        connect(m_axis, &AxisBase::xAxisChanged, this, &PlotBase::triggerUpdate);
        connect(m_axis, &AxisBase::yAxisChanged, this, &PlotBase::triggerUpdate);

        m_axis->registerPlot(this);

    } else {
        qWarning() << Q_FUNC_INFO << "Plot is not a descendant of any Axis. It may not behave as expected.";
    }
    emit axisChanged(arg);
}

void PlotBase::triggerUpdate()
{
    update();
    if (m_canvas)
        m_canvas->update();
}

void PlotBase::updateAxis()
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

void PlotBase::setDataLimits(QRectF arg)
{
    if (m_dataLimits == arg) return;
    m_dataLimits = arg;
    emit dataLimitsChanged(arg);
    triggerUpdate();
}
