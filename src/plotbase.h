#ifndef PLOT_H
#define PLOT_H

#include <QQuickItem>
#include <QPainter>
#include "plotdata.h"
#include "axisbase.h"
#include "nutmegobject.h"

#include "../QKDTree/QKDTree/QKDTree.h"

class AxisBase;
class PlotBase : public QQuickItem, public NutmegObject
{
    Q_OBJECT
    Q_PROPERTY(AxisBase* axis READ axis NOTIFY axisChanged)
    Q_PROPERTY(QString handle READ handle WRITE setHandle NOTIFY handleChanged)
    Q_PROPERTY(QQuickPaintedItem* canvas READ canvas WRITE setCanvas NOTIFY canvasChanged)

    Q_PROPERTY(QRectF dataLimits READ dataLimits NOTIFY dataLimitsChanged)

public:
    explicit PlotBase(QQuickItem *parent = 0);
    AxisBase* axis() const;

    QString handle() const;
    void setHandle(QString arg);

    QQuickPaintedItem* canvas() const;
    void setCanvas(QQuickPaintedItem* arg);

    Q_INVOKABLE QString map(QString prop);

    Q_INVOKABLE QPointF itemToData(QPointF point);
    Q_INVOKABLE QPointF dataToItem(QPointF point);

    Q_INVOKABLE QList<QPointF> itemToDataList(QList<QPointF> points);
    Q_INVOKABLE QList<QPointF> dataToItemList(QList<QPointF> points);

    QRectF dataLimits() const;

signals:
    void axisChanged(AxisBase* arg);
    void handleChanged(QString arg);
    void canvasChanged(QQuickPaintedItem* arg);
    void dataLimitsChanged(QRectF arg);

public slots:
    void triggerUpdate();
    void updateAxis();

protected slots:
    virtual void updateDataLimits() {}

protected:
    void setDataLimits(QRectF arg);
    QRectF m_dataLimits;

private:
    void setAxis(AxisBase* arg);

    AxisBase* m_axis;
    QString m_handle;
    QQuickPaintedItem* m_canvas;
};

#endif // PLOT_H
