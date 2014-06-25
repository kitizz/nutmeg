#include "imagecanvas.h"
#include "imageplot.h"

ImageCanvas::ImageCanvas(QQuickItem *parent) :
    PlotCanvas(parent)
{
}

void ImageCanvas::paint(QPainter *painter)
{
    ImagePlot *plot = qobject_cast<ImagePlot*>(parent());
    if (!plot) return;

    AxisBase *monAxis = plot->axis();

    // Is this copying the whole pixmap?
    QPixmap pix = plot->pixmap();

    QRectF target = this->boundingRect();
    qreal top = target.top();
    target.setTop(target.bottom());
    target.setBottom(top);
    painter->drawPixmap(target, pix, monAxis->limits());
}
