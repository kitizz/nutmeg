#include "canvasplotcanvas.h"

#include "canvasplot.h"
#include "canvastext.h"

CanvasPlotCanvas::CanvasPlotCanvas(QQuickItem *parent) :
    PlotCanvas(parent)
{
}

void CanvasPlotCanvas::paint(QPainter *painter)
{
    CanvasPlot *plot = qobject_cast<CanvasPlot*>(parent());
    if (!plot) return;

    Axis2DBase *monAxis = plot->axis2d();
    if (!monAxis)
        return; // Funky data

    QRectF lim = monAxis->limits();
    if (lim.width() == 0 || lim.height() == 0)
        return;

    // Transform the plot coords to view coords
    qreal scaleX = width()/(lim.width());
    qreal scaleY = height()/(lim.height());
    qreal tx = 0;
    qreal ty = 0;
    if (monAxis->xAxis()->inverted()) {
        scaleX *= -1;
        tx = width();
    }
    if (!monAxis->yAxis()->inverted()) {
        scaleY *= -1;
        ty = height();
    }

    QTransform tran;
    tran.translate(tx, ty).scale(scaleX, scaleY).translate(-lim.x(), -lim.y());

    foreach (CanvasShape *shape, plot->shapes()) {
        shape->paint(painter, tran, lim);
    }
}
