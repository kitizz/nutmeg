#ifndef PLOTCANVAS_H
#define PLOTCANVAS_H

#include "../../nutmeglibdecl.h"
#include <QQuickPaintedItem>

class NUTMEGLIB_EXPORT PlotCanvas : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(qreal scaling READ scaling WRITE setScaling NOTIFY scalingChanged)

public:
    explicit PlotCanvas(QQuickItem *parent = 0);

    virtual void paint(QPainter *painter) = 0;

    qreal scaling() const;
    void setScaling(qreal arg);

signals:
    void scalingChanged(qreal arg);

protected:
    bool updateTriggered();
    void resetTrigger();

public slots:
    void triggerUpdate();

private:
    qreal m_scaling;
    bool m_updateTriggered;
};

#endif // PLOTCANVAS_H
