#ifndef XYPLOT_H
#define XYPLOT_H

#include "plot2dbase.h"
#include "../../util/util.h"
#include "../../util/ndarray.h"

class NUTMEGLIB_EXPORT XYPlot : public Plot2DBase
{
    Q_OBJECT
    Q_PROPERTY(NDArray xData READ xData WRITE setXData NOTIFY xDataChanged)
    Q_PROPERTY(NDArray yData READ yData WRITE setYData NOTIFY yDataChanged)

public:
    explicit XYPlot(QQuickItem *parent = 0);

    virtual NDArray &xData();
    virtual NDArray &yData();
    virtual void setXData(const NDArray &arg);
    virtual void setYData(const NDArray &arg);
//    QVariant xDataVar();
//    QVariant yDataVar();
    virtual int xSize() const;
    virtual int ySize() const;

    virtual int startIndexX() const;
    virtual int startIndexY() const;

    Q_INVOKABLE QPointF dataLocationAt(int index);
    Q_INVOKABLE QPointF frameLocationAt(int index);

    Q_INVOKABLE int nearestDataTo(const QPointF &dataLocation);

signals:
    void xDataChanged(const NDArray &arg);
    void yDataChanged(const NDArray &arg);

public slots:
    void setData(const NDArray &xVals, const NDArray &yVals);
    void setXDataFromList(QList<qreal> arg);
    void setYDataFromList(QList<qreal> arg);
    virtual QList<qreal> xDataList();
    virtual QList<qreal> yDataList();

protected slots:
    virtual void updateDataLimits();
    void updateTree();

protected:
    bool m_settingData;

private:
    bool m_xDataSet;
    NDArray m_xData;
    NDArray m_yData;
    // TODO: Implement my own KDTree - QKDTree had weird problems...
//    QKDTree *m_tree;
    bool m_treeDirty;

};

#endif // XYPLOT_H
