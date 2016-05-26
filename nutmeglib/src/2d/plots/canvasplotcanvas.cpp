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

    QTransform tran = Util::plotToView(QSizeF(plot->width(), plot->height()), lim, monAxis->xAxis()->inverted(), monAxis->yAxis()->inverted());
//    tran.translate(tx, ty).scale(scaleX, scaleY).translate(-lim.x(), -lim.y());

    foreach (CanvasShape *shape, plot->shapes()) {
        shape->paint(painter, tran, lim);
    }
}
