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
    qreal x1, y1, x2, y2;
    for (int i=0; i<sx.length(); ++i) {
        x1 = sx[i]; y1 = sy[i];
        x2 = ex[i]; y2 = ey[i];
//        rectSlice(x1, y1, x2, y2, lim, valid);
//        if (valid)
//            lines << tran.map( QLineF(x1, y1, x2, y2) );
    }

    preparePainter(painter, plot);
    painter->drawLines(QVector<QLineF>::fromList(lines));
}

