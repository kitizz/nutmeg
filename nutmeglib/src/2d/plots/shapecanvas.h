#ifndef SHAPECANVAS_H
#define SHAPECANVAS_H

#include <QQuickPaintedItem>
#include "plotcanvas.h"

class NUTMEGLIB_EXPORT ShapeCanvas : public PlotCanvas
{
    Q_OBJECT
public:
    explicit ShapeCanvas(QQuickItem *parent = 0);

signals:

public slots:

};

#endif // SHAPECANVAS_H
