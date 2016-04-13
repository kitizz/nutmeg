#include "canvasrect.h"

CanvasRect::CanvasRect(QQuickItem *parent) :
    CanvasShape(parent)
{
}

void CanvasRect::paint(QPainter *painter, const QTransform &tran, const QRectF &limits)
{
    QRectF plotRect = bounds();
    // Let's not waste time here...
    if (!limits.intersects(plotRect)) return;

    // TODO: This boilerplate style stuff to a CanvasShape::applyStyle(QPainter*) func?
    QPen pen;
    LineSpec *ln = lineSpec();
    pen.setColor(ln->color());
    pen.setWidthF(ln->width());

    QString style = ln->style();
    if (style == ".") {
        pen.setStyle(Qt::SolidLine);
    } else {
        pen.setStyle( LineSpec::styleMap[style] );
    }

    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(fillSpec()->color());

    painter->setRenderHint(QPainter::Antialiasing);

    // Only draw the part of the rect in the view
    QRectF rect = limits.intersected(plotRect);
    // Figure out which sides still need lines...
    QVector<QLineF> lines;
    if (rect.left() == plotRect.left())
        lines << QLineF(rect.bottomLeft(), rect.topLeft());
    if (rect.right() == plotRect.right())
        lines << QLineF(rect.bottomRight(), rect.topRight());
    if (rect.top() == plotRect.top())
        lines << QLineF(rect.topLeft(), rect.topRight());
    if (rect.bottom() == plotRect.bottom())
        lines << QLineF(rect.bottomLeft(), rect.bottomRight());

    // Map the fill and lines
    QRectF mappedRect = tran.mapRect(rect);
    for (int i=0; i<lines.length(); ++i) {
        lines[i] = tran.map(lines[i]);
    }

    // Draw the rect
    painter->setPen(Qt::NoPen);
    painter->setBrush(brush);
    painter->drawRect(mappedRect);

    // Draw the outline
    painter->setBrush(Qt::NoBrush);
    painter->setPen(pen);
    painter->drawLines(lines);
}

//QRectF CanvasRect::rect() const
//{
//    return m_rect;
//}

//void CanvasRect::setRect(QRectF arg)
//{
//    if (m_rect == arg) return;
//    m_rect = arg;
//    setBounds(arg);
//    emit rectChanged(arg);
//}
