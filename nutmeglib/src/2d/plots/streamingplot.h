#ifndef STREAMINGPLOT_H
#define STREAMINGPLOT_H

#include "lineplot.h"

class StreamingPlot : public LinePlot
{
    Q_OBJECT
    Q_PROPERTY(int size READ size WRITE setSize NOTIFY sizeChanged)

public:
    explicit StreamingPlot(QQuickItem *parent = 0);

    Q_INVOKABLE void appendXY(qreal x, qreal y);
    Q_INVOKABLE void extendXY(QVariantList x, QVariantList y);
    Q_INVOKABLE void appendX(qreal x);
    Q_INVOKABLE void extendX(QVariantList xs);
    Q_INVOKABLE void appendY(qreal y);
    Q_INVOKABLE void extendY(QVariantList ys);

    int size() const;
    virtual int xSize() const;
    virtual int ySize() const;
    virtual void setXData(const NDArray &arg);
    virtual void setYData(const NDArray &arg);

    virtual int startIndexX() const;
    virtual int startIndexY() const;


public slots:
    void setSize(int size);

protected slots:
    virtual void updateDataLimits();

signals:
    void sizeChanged(int size);

private:
    int m_size;
    int m_datacount;
    int m_startx;
    int m_starty;
};

#endif // STREAMINGPLOT_H
