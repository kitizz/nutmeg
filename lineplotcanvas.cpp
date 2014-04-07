#include "lineplotcanvas.h"
#include "lineplot.h"

LinePlotCanvas::LinePlotCanvas(QQuickItem *parent) :
    PlotCanvas(parent)
{
}

void LinePlotCanvas::paint(QPainter *painter)
{
    LinePlot *plot = qobject_cast<LinePlot*>(parent());
    if (!plot) return;

    AxisBase *monAxis = plot->axis();
    QList<qreal> xData = plot->xData(), yData = plot->yData();
    if (!monAxis || xData.length() != yData.length())
        return; // Funky data

    QRectF dataLim = plot->dataLimits();
    // Need to frame the data based on the limits
    qreal minX = monAxis->minX();
    qreal minY = monAxis->minY();
    qreal maxX = monAxis->maxX();
    qreal maxY = monAxis->maxX();
    if (minX == -Inf) minX = dataLim.left();
    if (minY == -Inf) minY = dataLim.top();
    if (maxX == Inf) maxX = dataLim.right();
    if (maxY == Inf) maxY = dataLim.bottom();

    qreal scaleX = width()/(maxX - minX);
    qreal scaleY = height()/(maxY - minY);

    QPolygonF line;
    for (int i=0; i<xData.length(); ++i) {
        // Get it into workspace coords
        qreal px = xData[i], py = yData[i];
        line << QPointF(scaleX*(px - minX), scaleY*(py - minY));
    }

    painter->setRenderHint(QPainter::Antialiasing);

    QPen penn = QPen();
    penn.setColor( plot->lineColor() );
    penn.setStyle( LineSpec::styleMap[plot->lineStyle()] );
    penn.setWidthF( plot->lineWidth() );

    painter->setPen(penn);
    painter->drawPolyline(line);
}
