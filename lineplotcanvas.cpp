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

//    qDebug() << "Plotting:" << plot->handle() << monAxis->dataLimits();
    QRectF dataLim = monAxis->dataLimits();
    // Need to frame the data based on the limits
    qreal minX = monAxis->minX();
    qreal minY = monAxis->minY();
    qreal maxX = monAxis->maxX();
    qreal maxY = monAxis->maxY();
//    qDebug() << "Init limits:" << "(" << minX << maxX << minY << maxY << ")";
    if (minX == -Inf) minX = dataLim.left();
    if (minY == -Inf) minY = dataLim.top();
    if (maxX == Inf) maxX = dataLim.right();
    if (maxY == Inf) maxY = dataLim.bottom();

    qreal scaleX = width()/(maxX - minX);
    qreal scaleY = height()/(maxY - minY);

//    qDebug() << "Size, scale:" << "(" << minX << maxX << minY << maxY << ")" << scaleX << scaleY;
    QPolygonF line;
    for (int i=0; i<yData.length(); ++i) {
        // Get it into workspace coords
        qreal px = xData[i];
        qreal py = yData[i];
        line << QPointF(scaleX*(px - minX), scaleY*(py - minY));
    }

//    qDebug() << "Defined poly:" << line.size();

    painter->setRenderHint(QPainter::Antialiasing);

    QPen penn = QPen();
    penn.setColor( plot->line()->color() );
    penn.setStyle( LineSpec::styleMap[plot->line()->style()] );
    penn.setWidthF( plot->line()->width() );

    painter->setPen(penn);
    painter->drawPolyline(line);
//    qDebug() << "Done Plotting:" << plot->handle();
}
