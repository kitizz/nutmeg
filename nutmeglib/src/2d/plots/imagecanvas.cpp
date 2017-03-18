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

    painter->save();
//    painter->setTransform(QTransform()); // Required sinced Qt 5.6

    // Transform the plot coords to view coords
    qreal scaleX = 1, scaleY = 1;
    qreal scaleOffsetX = 0, scaleOffsetY = 0;
    if (monAxis->xAxis()->inverted()) {
        scaleX *= -1;
        scaleOffsetX = width();
    }
    if (!monAxis->yAxis()->inverted()) {
        scaleY *= -1;
        scaleOffsetY = height();
    }
    // The following should still be fine for PDF output...
    painter->translate(scaleOffsetX, scaleOffsetY);
    painter->scale(scaleX, scaleY);

    // Now let's work out all the image clipping stuff...
    const qreal offsetX = plot->xOffset(), offsetY = plot->yOffset();
    QRectF axlim = monAxis->limits();  // Source "view" of image without transformation
    qreal w = axlim.width()/plot->xScale();
    qreal h = axlim.height()/plot->yScale();
    qreal x = (axlim.x() - offsetX)/plot->xScale();
    qreal y = (axlim.y() - offsetY)/plot->yScale();
    QRectF src(x, y, w, h);

    // Is this copying the whole pixmap?
    QPixmap pix = plot->pixmap();
    // Does this automatically clip pixels outside src image for performance?
    painter->drawPixmap(this->boundingRect(), pix, src);
    painter->restore();
}
