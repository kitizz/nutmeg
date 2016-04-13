#ifndef LINESEGMENTPLOTCANVAS_H
#define LINESEGMENTPLOTCANVAS_H

#include "lineplotcanvas.h"

class NUTMEGLIB_EXPORT LineSegmentPlotCanvas : public LinePlotCanvas
{
    Q_OBJECT

public:
    LineSegmentPlotCanvas(QQuickItem *parent=0);
    ~LineSegmentPlotCanvas();

    virtual void paint(QPainter *painter);
};

#endif // LINESEGMENTPLOTCANVAS_H
