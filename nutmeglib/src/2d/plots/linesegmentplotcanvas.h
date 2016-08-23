#ifndef LINESEGMENTPLOTCANVAS_H
#define LINESEGMENTPLOTCANVAS_H

#include "lineplotrenderer.h"

class NUTMEGLIB_EXPORT LineSegmentPlotCanvas : public LinePlotRenderer
{
public:
    LineSegmentPlotCanvas(LinePlot *parent=0);
    ~LineSegmentPlotCanvas();

//    virtual void paint(QPainter *painter);
};

#endif // LINESEGMENTPLOTCANVAS_H
