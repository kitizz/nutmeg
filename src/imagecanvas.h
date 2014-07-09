#ifndef IMAGECANVAS_H
#define IMAGECANVAS_H

#include <QQuickPaintedItem>
#include "plotcanvas.h"

class ImageCanvas : public PlotCanvas
{
    Q_OBJECT
public:
    explicit ImageCanvas(QQuickItem *parent = 0);

    void paint(QPainter *painter);

signals:

public slots:

};

#endif // IMAGECANVAS_H
