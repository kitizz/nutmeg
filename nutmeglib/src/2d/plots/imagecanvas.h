#ifndef IMAGECANVAS_H
#define IMAGECANVAS_H

#include <QQuickPaintedItem>
#include "plotcanvas.h"

// TODO: Rename to ImagePlotCanvas
class NUTMEGLIB_EXPORT ImageCanvas : public PlotCanvas
{
public:
    explicit ImageCanvas(PlotBase *parent = 0);

    void paint(QPainter *painter);

};

#endif // IMAGECANVAS_H
