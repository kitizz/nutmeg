#ifndef CANVASPLOTCANVAS_H
#define CANVASPLOTCANVAS_H

#include "plotcanvas.h"

class NUTMEGLIB_EXPORT CanvasPlotCanvas : public PlotCanvas
{
public:
    explicit CanvasPlotCanvas(PlotBase *parent = 0);

    virtual void paint(QPainter* painter);

};

#endif // CANVASPLOTCANVAS_H
