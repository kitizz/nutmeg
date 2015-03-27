#ifndef LINEPLOT_H
#define LINEPLOT_H

#include "xyplot.h"
#include "axis2dbase.h"
#include "linespec.h"
#include <QPolygonF>
#include <QMap>

class LinePlot : public XYPlot
{
    Q_OBJECT
    Q_PROPERTY(LineSpec* line READ line NOTIFY lineChanged)

public:
    explicit LinePlot(QQuickItem *parent = 0);

    LineSpec* line() const;

signals:
    void lineChanged(LineSpec* arg);

private:
    QList<QPointF> m_points;
    LineSpec *m_line;
};

#endif // LINEPLOT_H
