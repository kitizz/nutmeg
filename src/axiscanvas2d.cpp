#include "axiscanvas2d.h"
#include <QGuiApplication>
#include <QScreen>

#include <QPen>

AxisCanvas2D::AxisCanvas2D(QQuickItem *parent)
    : QQuickPaintedItem(parent)
    , m_axis(0)
    , m_plotRect(QRectF())
    , m_scaling(QGuiApplication::primaryScreen()->devicePixelRatio())
{
    setRenderTarget(QQuickPaintedItem::FramebufferObject);
//    connect(this, &AxisCanvas2D::plotRectChanged, this, &AxisCanvas2D::update);
}

void AxisCanvas2D::paint(QPainter *painter)
{
    QRectF lim = m_axis->limits();

    QList<qreal> xMajor, yMajor;

    foreach (QVariant tick, m_axis->xAxis()->majorTicks()) {
        qreal xNorm = (tick.toReal() - lim.left()) / lim.width();
        if (m_axis->xAxis()->inverted())
            xNorm = 1 - xNorm;
        xMajor << m_plotRect.left() + m_plotRect.width()*xNorm;
    }

    foreach (QVariant tick, m_axis->yAxis()->majorTicks()) {
        qreal yNorm = (tick.toReal() - lim.top()) / lim.height();
        if (m_axis->yAxis()->inverted())
            yNorm = 1 - yNorm;
        yMajor << m_plotRect.bottom() - m_plotRect.height()*yNorm;
    }

    drawGrid(xMajor, yMajor, painter);
    drawXTicks(xMajor, painter);
    drawYTicks(yMajor, painter);
}



AxisBase *AxisCanvas2D::axis() const
{
    return m_axis;
}

void AxisCanvas2D::setAxis(AxisBase *arg)
{
    if (m_axis == arg) return;

    if (m_axis) {
        m_axis->disconnect(this);
        m_axis->xAxis()->disconnect(this);
        m_axis->yAxis()->disconnect(this);
    }

    m_axis = arg;

    if (m_axis) {
        connect(m_axis, &AxisBase::limitsChanged, this, &AxisCanvas2D::triggerUpdate);
        connect(m_axis->xAxis(), &AxisSpec::majorTicksChanged, this, &AxisCanvas2D::triggerUpdate);
        connect(m_axis->xAxis(), &AxisSpec::minorTicksChanged, this, &AxisCanvas2D::triggerUpdate);
        connect(m_axis->yAxis(), &AxisSpec::majorTicksChanged, this, &AxisCanvas2D::triggerUpdate);
        connect(m_axis->yAxis(), &AxisSpec::minorTicksChanged, this, &AxisCanvas2D::triggerUpdate);
    }
    emit axisChanged(arg);
}

QRectF AxisCanvas2D::plotRect() const
{
    return m_plotRect;
}

void AxisCanvas2D::setPlotRect(QRectF arg)
{
    if (m_plotRect == arg) return;
    m_plotRect = arg;
    update();
    emit plotRectChanged(arg);
}

qreal AxisCanvas2D::scaling() const
{
    return m_scaling;
}

void AxisCanvas2D::setScaling(qreal arg)
{
    if (m_scaling == arg) return;
    m_scaling = arg;
    emit scalingChanged(arg);
}

void AxisCanvas2D::triggerUpdate()
{
    update();
}

void AxisCanvas2D::drawXTicks(QList<qreal> major, QPainter *painter)
{
    QVector<QPointF> lines;

    qreal majorSize = m_axis->xAxis()->majorTickSize();
    qreal minorSize = m_axis->xAxis()->minorTickSize();

    // Calculate how to draw the tick directions along the x-axis
    AxisSpec::TickDirection xDir = m_axis->xAxis()->tickDir();
    qreal y1 = m_plotRect.bottom(), y2 = m_plotRect.bottom();
    if (xDir == AxisSpec::In || xDir == AxisSpec::InOut)
        y1 -= majorSize;
    if (xDir == AxisSpec::Out || xDir == AxisSpec::InOut)
        y2 += majorSize;
    else
        y2 -= 1;

    foreach (qreal x, major) {
        lines << m_scaling*QPointF(x, y1);
        lines << m_scaling*QPointF(x, y2);
    }

    LineSpec *spec = m_axis->xAxis()->majorLine();
    QPen pen;
    pen.setColor(spec->color());
    pen.setWidth(spec->width());
    painter->setPen(pen);
    painter->drawLines(lines);
}

void AxisCanvas2D::drawYTicks(QList<qreal> major, QPainter *painter)
{
    QRectF lim = m_axis->limits();
    QVector<QPointF> lines;

    qreal majorSize = m_axis->yAxis()->majorTickSize();
    qreal minorSize = m_axis->yAxis()->minorTickSize();

    // Calculate how to draw the tick directions along the y-axis
    AxisSpec::TickDirection yDir = m_axis->yAxis()->tickDir();
    qreal x1 = m_plotRect.left(), x2 = m_plotRect.left();
    if (yDir == AxisSpec::In || yDir == AxisSpec::InOut)
        x1 += majorSize;
    if (yDir == AxisSpec::Out || yDir == AxisSpec::InOut)
        x2 -= majorSize;
    else
        x2 += 1;

    foreach (qreal y, major) {
        lines << m_scaling*QPointF(x1, y);
        lines << m_scaling*QPointF(x2, y);
    }

    LineSpec *spec = m_axis->yAxis()->majorLine();
    QPen pen;
    pen.setColor(spec->color());
    pen.setWidth(spec->width());
    painter->setPen(pen);
    painter->drawLines(lines);
}

void AxisCanvas2D::drawGrid(QList<qreal> xMajor, QList<qreal> yMajor, QPainter *painter)
{
    QVector<QPointF> lines;

    if (m_axis->grid()->axes() & AxisGrid::X) {
        foreach (qreal x, xMajor) {
            lines << m_scaling*QPoint(x, m_plotRect.top());
            lines << m_scaling*QPoint(x, m_plotRect.bottom());
        }
    }
    if (m_axis->grid()->axes() & AxisGrid::Y) {
        foreach (qreal y, yMajor) {
            lines << m_scaling*QPoint(m_plotRect.left(), y);
            lines << m_scaling*QPoint(m_plotRect.right(), y);
        }
    }

    LineSpec *spec = m_axis->grid()->majorLine();
    QPen pen;
    pen.setColor(spec->color());
    pen.setWidth(spec->width());
    pen.setStyle(LineSpec::styleMap[spec->style()]);
    painter->setPen(pen);
    painter->drawLines(lines);
}
