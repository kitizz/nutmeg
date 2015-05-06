#ifndef CANVASLINE_H
#define CANVASLINE_H

#include "canvasshape.h"
#include <QPainter>

class CanvasLine : public CanvasShape
{
    Q_OBJECT
    Q_PROPERTY(QList<qreal> xData READ xData WRITE setXData NOTIFY xDataChanged)
    Q_PROPERTY(QList<qreal> yData READ yData WRITE setYData NOTIFY yDataChanged)

public:
    explicit CanvasLine(QQuickItem *parent = 0);

    virtual void paint(QPainter* painter, const ::QTransform &tran, const QRectF &limits);

    QList<qreal> xData() const;
    void setXData(QList<qreal> arg);

    QList<qreal> yData() const;
    void setYData(QList<qreal> arg);

signals:
    void xDataChanged(QList<qreal> arg);
    void yDataChanged(QList<qreal> arg);

public slots:

protected:
    void updateDataLimits();

private:
    QList<qreal> m_xData;
    QList<qreal> m_yData;

};

#endif // CANVASLINE_H
