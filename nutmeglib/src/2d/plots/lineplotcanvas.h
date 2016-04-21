#ifndef LINEPLOTCANVAS_H
#define LINEPLOTCANVAS_H

#include <QQuickPaintedItem>
#include "plotcanvas.h"

class QPainterPath;
class LinePlot;
class NUTMEGLIB_EXPORT LinePlotCanvas : public PlotCanvas
{
    Q_OBJECT
public:
    explicit LinePlotCanvas(QQuickItem *parent = 0);

    virtual void paint(QPainter *painter);
    void updatePolish();

    /// \internal Be careful, modifies underlying data.
    static void rectSlice(qreal &x1, qreal &y1, qreal &x2, qreal &y2, const QRectF &r, bool &valid, bool &sliceStart, bool &sliceEnd);
    qreal pointOnLine(QPointF point, QPointF l1, QPointF l2);
    QPen preparePainter(QPainter *painter, LinePlot *plot);

    static void drawLineSlice(
            QPainterPath *path,
            qreal x1, qreal y1, qreal x2, qreal y2,
            qreal tx, qreal ty, qreal sx, qreal sy, const QRectF &r,
            bool &sliceEnd);

signals:

public slots:

private:
    static QPointF transformPoint(const QPointF &p, qreal tx, qreal ty, qreal sx, qreal sy, qreal limx, qreal limy);
    static inline void transformPoint(qreal &x, qreal &y, qreal &x_out, qreal &y_out, qreal tx, qreal ty, qreal sx, qreal sy, qreal limx, qreal limy);

};

#endif // LINEPLOTCANVAS_H
