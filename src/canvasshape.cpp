#include "canvasshape.h"

CanvasShape::CanvasShape(QQuickItem *parent)
    : QQuickItem(parent)
    , m_lineSpec(new LineSpec())
    , m_fillSpec(new FillSpec())
    , m_bounds(QRectF())
    , m_plot(0)
    , m_settingBounds(false)
    , m_destroying(false)
{
    QMap<QString, QString> props;
    props.insert("line", "line");
    props.insert("fill", "fill");
    registerProperties(props);

    connect(this, &QQuickItem::parentChanged, this, &CanvasShape::findPlot);

    connect(this, &QQuickItem::xChanged, this, &CanvasShape::updateDataLimits);
    connect(this, &QQuickItem::yChanged, this, &CanvasShape::updateDataLimits);
    connect(this, &QQuickItem::widthChanged, this, &CanvasShape::updateDataLimits);
    connect(this, &QQuickItem::heightChanged, this, &CanvasShape::updateDataLimits);
}

CanvasShape::~CanvasShape()
{
    qDebug() << "Destroying:" << this;
    m_plot->deregisterShape(this);
    m_destroying = true;
}

LineSpec *CanvasShape::lineSpec() const
{
    return m_lineSpec;
}

FillSpec *CanvasShape::fillSpec() const
{
    return m_fillSpec;
}

QRectF CanvasShape::bounds() const
{
    return m_bounds;
}

void CanvasShape::setBounds(QRectF arg)
{
    if (m_bounds == arg) return;
    m_bounds = arg;

    m_settingBounds = true;
    setX(arg.x());
    setY(arg.y());
    setWidth(arg.width());
    setHeight(arg.height());

    emit boundsChanged(arg);
    m_settingBounds = false;
}

CanvasPlot *CanvasShape::plot() const
{
    return m_plot;
}

void CanvasShape::setPlot(CanvasPlot *arg)
{
    if (m_plot == arg) return;
    m_plot = arg;
    emit plotChanged(arg);
}

void CanvasShape::updateDataLimits()
{
    if (m_destroying || m_settingBounds) return;
    QRectF newRect = QRectF(x(), y(), width(), height());

    if (newRect == m_bounds) return;
    m_bounds = newRect;
    emit boundsChanged(newRect);
}

void CanvasShape::findPlot()
{
    if (m_destroying) return;

    QObject *parent = this;
    while(parent->parent()) {
        parent = parent->parent();
        CanvasPlot *plt = qobject_cast<CanvasPlot*>(parent);
        if (!plt) continue;

        // We found a CanvasPlot!
        plt->registerShape(this);
        setPlot(plt);
        break;
    }
}
