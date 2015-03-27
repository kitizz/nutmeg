#ifndef CANVASRECT_H
#define CANVASRECT_H

#include "canvasshape.h"


class CanvasRect : public CanvasShape
{
    Q_OBJECT
//    Q_PROPERTY(QRectF rect READ rect WRITE setRect NOTIFY rectChanged)

public:
    explicit CanvasRect(QQuickItem *parent = 0);

    virtual void paint(QPainter *painter, const QTransform &tran, const QRectF &limits);

//    QRectF rect() const;
//    void setRect(QRectF arg);

signals:

//    void rectChanged(QRectF arg);

public slots:
//    void updateDataLimits();

private:
};

#endif // CANVASRECT_H
