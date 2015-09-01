#ifndef LINEPLOTCANVAS_H
#define LINEPLOTCANVAS_H

#include <QQuickPaintedItem>
#include "plotcanvas.h"

class LinePlot;
class LinePlotCanvas : public PlotCanvas
{
    Q_OBJECT
public:
    explicit LinePlotCanvas(QQuickItem *parent = 0);

    virtual void paint(QPainter *painter);
    void updatePolish();

    QLineF rectSlice(const QPointF &p1, const QPointF &p2, const QRectF &r, bool &valid);
    qreal pointOnLine(QPointF point, QPointF l1, QPointF l2);
    QPen preparePainter(QPainter *painter, LinePlot *plot);
signals:

public slots:

private:
    QPointF transformPoint(const QPointF &p, qreal tx, qreal ty, qreal sx, qreal sy, qreal limx, qreal limy);

};

#endif // LINEPLOTCANVAS_H
