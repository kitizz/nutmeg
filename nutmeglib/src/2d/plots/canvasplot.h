#ifndef CANVASPLOT_H
#define CANVASPLOT_H

#include <QQuickItem>
#include <QTimer>
#include "xyplot.h"
#include "canvasshape.h"

class CanvasShape;
class NUTMEGLIB_EXPORT CanvasPlot : public Plot2DBase
{
    Q_OBJECT
    Q_PROPERTY(QList<CanvasShape*> shapes READ shapes NOTIFY shapesChanged)

public:
    explicit CanvasPlot(QQuickItem *parent = 0);
    ~CanvasPlot();

    QList<CanvasShape*> shapes() const;
    Q_INVOKABLE void registerShape(CanvasShape *shape);
    Q_INVOKABLE void deregisterShape(CanvasShape *shape);

signals:
    void shapesChanged(QList<CanvasShape*> arg);

protected:
    virtual void updateDataLimits();

public slots:
    void queueUpdateDataLimits();

private:
    QSet<CanvasShape*> m_shapes;
    QTimer m_tim;

};

#endif // CANVASPLOT_H
