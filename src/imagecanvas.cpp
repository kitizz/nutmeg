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

    Axis2DBase *monAxis = plot->axis2d();

    // Transform the plot coords to view coords
    qreal scaleX = 1, scaleY = 1;
    qreal scaleOffsetX = 0, scaleOffsetY = 0;
    if (monAxis->xAxis()->inverted()) {
        scaleX = -1;
        scaleOffsetX = width();
    }
    if (!monAxis->yAxis()->inverted()) {
        scaleY = 1;
        scaleOffsetY = height();
    }
    painter->translate(scaleOffsetX, scaleOffsetY);
    painter->scale(scaleX, scaleY);

    qDebug() << "Image Limits:" << monAxis->limits();

    // Is this copying the whole pixmap?
    QPixmap pix = plot->pixmap();
    painter->drawPixmap(this->boundingRect(), pix, monAxis->limits());
}
