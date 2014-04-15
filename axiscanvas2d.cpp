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
//    qDebug() << "Drawing text in axisCanvas" << m_scaling;
    QRectF lim = m_axis->limits();

    qreal majorSize = 5, minorSize = 2;
    QVector<QPointF> lines;

    foreach (QVariant tick, m_axis->xAxis()->majorTicks()) {
        qreal x = m_plotRect.left() + m_plotRect.width() * (tick.toReal() - lim.left()) / lim.width();
        lines << m_scaling*QPointF(x, m_plotRect.bottom());
        lines << m_scaling*QPointF(x, m_plotRect.bottom() + majorSize);
    }

//    lines.clear();
    foreach (QVariant tick, m_axis->yAxis()->majorTicks()) {
        qreal y = m_plotRect.bottom() - m_plotRect.height() * (tick.toReal() - lim.top()) / lim.height();
        lines << m_scaling*QPointF(m_plotRect.left(), y);
        lines << m_scaling*QPointF(m_plotRect.left() - majorSize, y);
    }

    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidth(2);
    painter->setPen(pen);
    painter->drawLines(lines);
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
