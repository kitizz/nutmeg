#include "linesegmentplotcanvas.h"

#include "linesegmentplot.h"

LineSegmentPlotCanvas::LineSegmentPlotCanvas(QQuickItem *parent)
    : LinePlotCanvas(parent)
{

}

LineSegmentPlotCanvas::~LineSegmentPlotCanvas()
{

}

void LineSegmentPlotCanvas::paint(QPainter *painter)
{
    if (!updateTriggered())
        return;
    resetTrigger();

    LineSegmentPlot *plot = qobject_cast<LineSegmentPlot*>(parent());
    if (!plot) return;

    Axis2DBase *monAxis = plot->axis2d();
    QList<qreal> sx = plot->startX(), sy = plot->startY();
    QList<qreal> ex = plot->endX(), ey = plot->endY();
    if (!monAxis ||
            sx.length() != sy.length() ||
            sx.length() != ex.length() ||
            sx.length() != ey.length() ||
            sx.length() == 0)
        return; // Funky data

    QRectF lim = monAxis->limits();
    QTransform tran = Util::plotToView(QSizeF(width(), height()), lim, monAxis->xAxis()->inverted(), monAxis->yAxis()->inverted());

    QList<QLineF> lines;
    bool valid;
    // Get the segments into screen coords
    for (int i=0; i<sx.length(); ++i) {
        const QPointF p1(sx[i], sy[i]), p2(ex[i], ey[i]);
        const QLineF sliced = rectSlice(p1, p2, lim, valid);
        if (valid)
            lines << tran.map( sliced );
    }

    preparePainter(painter, plot);
    painter->drawLines(QVector<QLineF>::fromList(lines));
}

