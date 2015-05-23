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
    void updatePolish();

    QLineF rectSlice(QPointF p1, QPointF p2, QRectF r);
    qreal pointOnLine(QPointF point, QPointF l1, QPointF l2);
signals:

public slots:

private:
    QPointF transformPoint(const QPointF &p, qreal tx, qreal ty, qreal sx, qreal sy, qreal limx, qreal limy);

};

#endif // LINEPLOTCANVAS_H
