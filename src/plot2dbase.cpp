#include "plot2dbase.h"
#include "axisbase.h"
#include <QPolygonF>
#include <QPainter>

#include <algorithm>

#include "../QKDTree/QVectorND/QVectorND.h"

Plot2DBase::Plot2DBase(QQuickItem *parent)
    : PlotBase(parent)
    , m_dataLimits(QRectF())
    , m_axis(0)
    , m_canvas(0)
{

}

Axis2DBase *Plot2DBase::axis2d() const
{
    return m_axis;
}

void Plot2DBase::print(QPainter *painter)
{
    if (!m_canvas)
        return;
    m_canvas->paint(painter);
}

PlotCanvas *Plot2DBase::canvas() const
{
    return m_canvas;
}

void Plot2DBase::setCanvas(PlotCanvas *arg)
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

void Plot2DBase::triggerUpdate()
{
    if (m_canvas)
        m_canvas->triggerUpdate();
//        m_canvas->update();
}

void Plot2DBase::updateAxis(AxisBase *oldAxis, AxisBase *newAxis)
{
    if (oldAxis) {
        oldAxis->disconnect(this);
        oldAxis->deregisterPlot(this);
    }

    m_axis = qobject_cast<Axis2DBase*>(newAxis);
    if (m_axis) {
        connect(m_axis, &Axis2DBase::minXChanged, this, &Plot2DBase::triggerUpdate);
        connect(m_axis, &Axis2DBase::minYChanged, this, &Plot2DBase::triggerUpdate);
        connect(m_axis, &Axis2DBase::maxXChanged, this, &Plot2DBase::triggerUpdate);
        connect(m_axis, &Axis2DBase::maxYChanged, this, &Plot2DBase::triggerUpdate);
        connect(m_axis, &Axis2DBase::dataLimitsChanged, this, &Plot2DBase::triggerUpdate);
        connect(m_axis, &Axis2DBase::xAxisChanged, this, &Plot2DBase::triggerUpdate);
        connect(m_axis, &Axis2DBase::yAxisChanged, this, &Plot2DBase::triggerUpdate);

        m_axis->registerPlot(this);

    } else {
        qWarning() << Q_FUNC_INFO << "Plot is not a descendant of any Axis. It may not behave as expected.";
    }
}

void Plot2DBase::setDataLimits(QRectF arg)
{
    if (m_dataLimits == arg) return;
    m_dataLimits = arg;
    qDebug() << Q_FUNC_INFO << "emit DataLimitsChanged" << this->handle();
    emit dataLimitsChanged(arg);
    triggerUpdate();
}
