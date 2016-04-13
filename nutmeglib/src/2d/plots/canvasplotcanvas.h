#ifndef CANVASPLOTCANVAS_H
#define CANVASPLOTCANVAS_H

#include "plotcanvas.h"

class NUTMEGLIB_EXPORT CanvasPlotCanvas : public PlotCanvas
{
    Q_OBJECT
public:
    explicit CanvasPlotCanvas(QQuickItem *parent = 0);

    virtual void paint(QPainter* painter);

signals:

public slots:

};

#endif // CANVASPLOTCANVAS_H
