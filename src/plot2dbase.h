#ifndef PLOT_H
#define PLOT_H

#include <QQuickItem>
#include <QPainter>
#include "plotdata.h"
#include "axis2dbase.h"
#include "plotbase.h"
#include "nutmegobject.h"
#include "plotcanvas.h"

#include "../QKDTree/QKDTree/QKDTree.h"

class Axis2DBase;
class Plot2DBase : public PlotBase
{
    Q_OBJECT
    Q_PROPERTY(PlotCanvas* canvas READ canvas WRITE setCanvas NOTIFY canvasChanged)
    Q_PROPERTY(QRectF dataLimits READ dataLimits NOTIFY dataLimitsChanged)

public:
    explicit Plot2DBase(QQuickItem *parent = 0);
    Axis2DBase *axis2d() const;

    PlotCanvas* canvas() const;
    void setCanvas(PlotCanvas* arg);

    Q_INVOKABLE QPointF itemToData(QPointF point);
    Q_INVOKABLE QPointF dataToItem(QPointF point);

    Q_INVOKABLE QList<QPointF> itemToDataList(QList<QPointF> points);
    Q_INVOKABLE QList<QPointF> dataToItemList(QList<QPointF> points);

    QRectF dataLimits() const;

signals:
    void canvasChanged(QQuickPaintedItem* arg);
    void dataLimitsChanged(QRectF arg);

public slots:
    void triggerUpdate();
    virtual void print(QPainter *painter);

protected slots:
    virtual void updateAxis(AxisBase *oldAxis, AxisBase* newAxis);
    virtual void updateDataLimits() {}

protected:
    void setDataLimits(QRectF arg);
    QRectF m_dataLimits;

private:
    Axis2DBase* m_axis;
    PlotCanvas* m_canvas;
};

#endif // PLOT_H
