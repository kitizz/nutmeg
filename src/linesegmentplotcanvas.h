#ifndef LINESEGMENTPLOTCANVAS_H
#define LINESEGMENTPLOTCANVAS_H

#include "lineplotcanvas.h"

class LineSegmentPlotCanvas : public LinePlotCanvas
{
    Q_OBJECT

public:
    LineSegmentPlotCanvas(QQuickItem *parent=0);
    ~LineSegmentPlotCanvas();

    virtual void paint(QPainter *painter);
};

#endif // LINESEGMENTPLOTCANVAS_H
