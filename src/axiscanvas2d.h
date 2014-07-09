#ifndef AXISCANVAS2D_H
#define AXISCANVAS2D_H

#include <QQuickPaintedItem>
#include "axisbase.h"

class AxisCanvas2D : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(AxisBase* axis READ axis WRITE setAxis NOTIFY axisChanged)
    Q_PROPERTY(QRectF plotRect READ plotRect WRITE setPlotRect NOTIFY plotRectChanged)
    Q_PROPERTY(qreal scaling READ scaling WRITE setScaling NOTIFY scalingChanged)

public:
    explicit AxisCanvas2D(QQuickItem *parent = 0);

    void paint(QPainter *painter);

    AxisBase* axis() const;
    void setAxis(AxisBase* arg);

    QRectF plotRect() const;
    void setPlotRect(QRectF arg);

    qreal scaling() const;
    void setScaling(qreal arg);

signals:
    void axisChanged(AxisBase* arg);
    void plotRectChanged(QRectF arg);
    void scalingChanged(qreal arg);

public slots:
    void triggerUpdate();

private:
    void drawXTicks(QList<qreal> major, QPainter *painter);
    void drawYTicks(QList<qreal> major, QPainter *painter);
    void drawGrid(QList<qreal> xMajor, QList<qreal> yMajor, QPainter *painter);
    AxisBase* m_axis;

    QRectF m_plotRect;
    qreal m_scaling;
};

#endif // AXISCANVAS2D_H
