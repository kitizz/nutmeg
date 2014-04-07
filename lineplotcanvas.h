#ifndef LINEPLOTCANVAS_H
#define LINEPLOTCANVAS_H

#include <QQuickPaintedItem>
#include "plotcanvas.h"

class LinePlotCanvas : public PlotCanvas
{
    Q_OBJECT
public:
    explicit LinePlotCanvas(QQuickItem *parent = 0);

    void paint(QPainter *painter);

signals:

public slots:

};

#endif // LINEPLOTCANVAS_H
