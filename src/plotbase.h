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


    Q_PROPERTY(QList<qreal> xData READ xData WRITE setXData NOTIFY xDataChanged)
    Q_PROPERTY(QList<qreal> yData READ yData WRITE setYData NOTIFY yDataChanged)
    Q_PROPERTY(QRectF dataLimits READ dataLimits NOTIFY dataLimitsChanged)

public:
    explicit PlotBase(QQuickItem *parent = 0);
    AxisBase* axis() const;

    QList<qreal> xData() const;
    QList<qreal> yData() const;
    void setXData(QList<qreal> arg);
    void setYData(QList<qreal> arg);

    Q_INVOKABLE QPointF dataLocationAt(int index);
    Q_INVOKABLE QPointF frameLocationAt(int index);

    QString handle() const;
    void setHandle(QString arg);

    QQuickPaintedItem* canvas() const;
    void setCanvas(QQuickPaintedItem* arg);

    QRectF dataLimits() const;

    Q_INVOKABLE QString map(QString prop);

    Q_INVOKABLE int nearestDataTo(const QPointF &dataLocation);

    Q_INVOKABLE QPointF itemToData(QPointF point);
    Q_INVOKABLE QPointF dataToItem(QPointF point);

    Q_INVOKABLE QList<QPointF> itemToDataList(QList<QPointF> points);
    Q_INVOKABLE QList<QPointF> dataToItemList(QList<QPointF> points);

signals:
    void xDataChanged(QList<qreal> arg);
    void yDataChanged(QList<qreal> arg);

    void dataLimitsChanged(QRectF arg);
    void axisChanged(AxisBase* arg);
    void handleChanged(QString arg);
    void canvasChanged(QQuickPaintedItem* arg);

public slots:
    void setData(QList<qreal> xVals, QList<qreal> yVals);
    void triggerUpdate();
    void updateAxis();

protected slots:
    void updateX();
    void updateData();
    void updateTree();

private:
    void setAxis(AxisBase* arg);
    bool dataIncreasing(QList<qreal> data);
    // TODO: The following functions should find their way to a static util library...
    QList<int>* medianArray(int N, int s=0, QList<int> *lst=0);
    int randInt(int low, int high);
    QList<int> *randomArray(int N);

    bool m_settingData;
    bool m_xDataSet;
    QList<qreal> m_xData;
    QList<qreal> m_yData;
    QKDTree *m_tree;
    bool m_treeDirty;
    QRectF m_dataLimits;

    AxisBase* m_axis;
    QString m_handle;
    QQuickPaintedItem* m_canvas;
};

#endif // PLOT_H
