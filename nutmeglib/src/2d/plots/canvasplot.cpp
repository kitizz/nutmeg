#include "canvasplot.h"

CanvasPlot::CanvasPlot(QQuickItem *parent)
    : Plot2DBase(parent)
    , m_shapes(QSet<CanvasShape*>())
{
    m_tim.setSingleShot(true);
    m_tim.setInterval(40);

    connect(&m_tim, &QTimer::timeout, [=](){ updateDataLimits(); });
}

CanvasPlot::~CanvasPlot()
{
    foreach(CanvasShape *shape, shapes()) {
        deregisterShape(shape);
        delete shape;
    }
}

QList<CanvasShape *> CanvasPlot::shapes() const
{
    return m_shapes.toList();
}

void CanvasPlot::registerShape(CanvasShape *shape) {
    qDebug() << "Registering shape with CanvasPlot" << shape;
//    qDebug() << "Shapes:" << m_shapes;
    shape->setPlot(this);
    connect(shape, &CanvasShape::boundsChanged, this, &CanvasPlot::queueUpdateDataLimits);
    m_shapes.insert(shape);

//    updateDataLimits();
    m_tim.start();
    emit shapesChanged(m_shapes.toList());
}

void CanvasPlot::deregisterShape(CanvasShape *shape) {
    m_shapes.remove(shape);

//    updateDataLimits();
    m_tim.start();
    emit shapesChanged(m_shapes.toList());
}

void CanvasPlot::queueUpdateDataLimits()
{
    if (!m_tim.isActive())
        m_tim.start();
}

void CanvasPlot::updateDataLimits()
{
    qreal minX = Inf, maxX = -Inf, minY = Inf, maxY = -Inf;
    foreach (CanvasShape* shape, m_shapes) {
        QRectF b = shape->bounds();
        qDebug() << "Shape Bounds:" << b;
        if (b.left() < minX)
            minX = b.left();
        if (b.right() > maxX)
            maxX = b.right();
        if (b.top() < minY)
            minY = b.top();
        if (b.bottom() > maxY)
            maxY = b.bottom();
    }
    qDebug() << "CanvasPlot.updateLimits:" << minX << minY << maxX << maxY;
    setDataLimits(QRectF(minX, minY, maxX-minX, maxY-minY));
}
