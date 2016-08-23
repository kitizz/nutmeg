#ifndef SHAPECANVAS_H
#define SHAPECANVAS_H

#include <QQuickPaintedItem>
#include "plotcanvas.h"

class NUTMEGLIB_EXPORT ShapeCanvas : public PlotCanvas
{
public:
    explicit ShapeCanvas(PlotBase *parent = 0);

};

#endif // SHAPECANVAS_H
