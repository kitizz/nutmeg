#ifndef IMAGECANVAS_H
#define IMAGECANVAS_H

#include <QQuickPaintedItem>
#include "plotcanvas.h"

// TODO: Rename to ImagePlotCanvas
class NUTMEGLIB_EXPORT ImageCanvas : public PlotCanvas
{
    Q_OBJECT
public:
    explicit ImageCanvas(QQuickItem *parent = 0);

    void paint(QPainter *painter);

signals:

public slots:

};

#endif // IMAGECANVAS_H
