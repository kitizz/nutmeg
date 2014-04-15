#ifndef PLOTCANVAS_H
#define PLOTCANVAS_H

#include <QQuickPaintedItem>

class PlotCanvas : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(qreal scaling READ scaling WRITE setScaling NOTIFY scalingChanged)

public:
    explicit PlotCanvas(QQuickItem *parent = 0);

    qreal scaling() const;
    void setScaling(qreal arg);

signals:
    void scalingChanged(qreal arg);

public slots:

private:
    qreal m_scaling;
};

#endif // PLOTCANVAS_H
