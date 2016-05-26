#ifndef LINESEGMENTPLOT_H
#define LINESEGMENTPLOT_H

#include "lineplot.h"

class NUTMEGLIB_EXPORT LineSegmentPlot : public LinePlot
{
    Q_OBJECT

//    Q_PROPERTY(QList<qreal> startX READ startX WRITE setStartX NOTIFY startXChanged)
    Q_PROPERTY(NDArray endX READ endX WRITE setEndX NOTIFY endXChanged)
//    Q_PROPERTY(NDArray startY READ startY WRITE setStartY NOTIFY startYChanged)
    Q_PROPERTY(NDArray endY READ endY WRITE setEndY NOTIFY endYChanged)

public:
    LineSegmentPlot(QQuickItem *parent = 0);
    ~LineSegmentPlot();

//    QList<qreal> startX() const;
//    void setStartX(QList<qreal> arg);

    NDArray &endX();
    void setEndX(const NDArray &arg);

//    QList<qreal> startY() const;
//    void setStartY(QList<qreal> arg);

    NDArray &endY();
    void setEndY(const NDArray &arg);

    int dataSize() const;
    virtual void updateDataLimits();

public slots:

protected slots:
//    void updateData();

signals:
//    void startXChanged(QList<qreal> arg);
    void endXChanged(const NDArray &arg);
//    void startYChanged(QList<qreal> arg);
    void endYChanged(const NDArray &arg);

private:
//    QList<qreal> m_startX;
    NDArray m_endX;
//    QList<qreal> m_startY;
    NDArray m_endY;
    int m_dataSize;

};

#endif // LINESEGMENTPLOT_H
