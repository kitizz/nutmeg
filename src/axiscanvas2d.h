#ifndef AXISCANVAS2D_H
#define AXISCANVAS2D_H

#include <QQuickPaintedItem>
#include "axis2dbase.h"

class AxisCanvas2D : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(Axis2DBase* axis READ axis WRITE setAxis NOTIFY axisChanged)
    Q_PROPERTY(QRectF plotRect READ plotRect WRITE setPlotRect NOTIFY plotRectChanged)
    Q_PROPERTY(qreal scaling READ scaling WRITE setScaling NOTIFY scalingChanged)

public:
    explicit AxisCanvas2D(QQuickItem *parent = 0);

    void paint(QPainter *painter);

    Axis2DBase* axis() const;
    void setAxis(Axis2DBase* arg);

    QRectF plotRect() const;
    void setPlotRect(QRectF arg);

    qreal scaling() const;
    void setScaling(qreal arg);

signals:
    void axisChanged(Axis2DBase* arg);
    void plotRectChanged(QRectF arg);
    void scalingChanged(qreal arg);

public slots:
    void triggerUpdate();

private:
    void drawXTicks(QList<qreal> major, QPainter *painter);
    void drawYTicks(QList<qreal> major, QPainter *painter);
    void drawGrid(QList<qreal> xMajor, QList<qreal> yMajor, QPainter *painter);
    Axis2DBase* m_axis;

    QRectF m_plotRect;
    qreal m_scaling;
};

#endif // AXISCANVAS2D_H
