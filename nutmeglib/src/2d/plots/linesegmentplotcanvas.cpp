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
    NDArray &sxA = plot->xData(),
            &syA = plot->yData(),
            &exA = plot->endX(),
            &eyA = plot->endY();

    int N = plot->dataSize();

    if (!monAxis || N == 0 ||
            sxA.type() == NDArray::Unknown || syA.type() == NDArray::Unknown ||
            exA.type() == NDArray::Unknown || eyA.type() == NDArray::Unknown)
        return; // Nothing to do

    NDArrayTyped<qreal> sx = sxA.convert<qreal>(),
                        sy = syA.convert<qreal>(),
                        ex = exA.convert<qreal>(),
                        ey = eyA.convert<qreal>();

    QRectF lim = monAxis->limits();

    // Get the transform params
    qreal tx, ty, scalex, scaley;//, limx, limy;
    Util::plotToView(plot->width(), plot->height(), lim, monAxis->xAxis()->inverted(), monAxis->yAxis()->inverted(),
                    tx, ty, scalex, scaley);
//    limx = lim.x();
//    limy = lim.y();

    // Allow the printer state to be restored
    painter->save();
    preparePainter(painter, plot);

    auto x1 = sx.begin();
    auto y1 = sy.begin();
    auto x2 = ex.begin();
    auto y2 = ey.begin();
    QPainterPath path;
    bool sliceEnd = true;

    for (int i = N - 1; i; --i, ++x1, ++y1, ++x2, ++y2) {
        sliceEnd = true;
        Util::drawLineSlice(&path, *x1, *y1, *x2, *y2, tx, ty, scalex, scaley, lim, sliceEnd);
    }
    painter->drawPath(path);

    painter->restore();
}

