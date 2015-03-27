#ifndef XYPLOT_H
#define XYPLOT_H

#include "plot2dbase.h"

class XYPlot : public Plot2DBase
{
    Q_OBJECT
    Q_PROPERTY(QList<qreal> xData READ xData WRITE setXData NOTIFY xDataChanged)
    Q_PROPERTY(QList<qreal> yData READ yData WRITE setYData NOTIFY yDataChanged)

public:
    explicit XYPlot(QQuickItem *parent = 0);

    QList<qreal> xData() const;
    QList<qreal> yData() const;
    void setXData(QList<qreal> arg);
    void setYData(QList<qreal> arg);

    Q_INVOKABLE QPointF dataLocationAt(int index);
    Q_INVOKABLE QPointF frameLocationAt(int index);

    Q_INVOKABLE int nearestDataTo(const QPointF &dataLocation);

signals:
    void xDataChanged(QList<qreal> arg);
    void yDataChanged(QList<qreal> arg);

public slots:
    void setData(QList<qreal> xVals, QList<qreal> yVals);

protected slots:
    void updateX();
    virtual void updateDataLimits();
    void updateTree();

private:
    bool dataIncreasing(QList<qreal> data);

    bool m_settingData;
    bool m_xDataSet;
    QList<qreal> m_xData;
    QList<qreal> m_yData;
    QKDTree *m_tree;
    bool m_treeDirty;

};

#endif // XYPLOT_H
