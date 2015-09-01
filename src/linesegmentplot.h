#ifndef LINESEGMENTPLOT_H
#define LINESEGMENTPLOT_H

#include "lineplot.h"

class LineSegmentPlot : public LinePlot
{
    Q_OBJECT

    Q_PROPERTY(QList<qreal> startX READ startX WRITE setStartX NOTIFY startXChanged)
    Q_PROPERTY(QList<qreal> endX READ endX WRITE setEndX NOTIFY endXChanged)
    Q_PROPERTY(QList<qreal> startY READ startY WRITE setStartY NOTIFY startYChanged)
    Q_PROPERTY(QList<qreal> endY READ endY WRITE setEndY NOTIFY endYChanged)

public:
    LineSegmentPlot(QQuickItem *parent = 0);
    ~LineSegmentPlot();

    QList<qreal> startX() const;
    void setStartX(QList<qreal> arg);

    QList<qreal> endX() const;
    void setEndX(QList<qreal> arg);

    QList<qreal> startY() const;
    void setStartY(QList<qreal> arg);

    QList<qreal> endY() const;
    void setEndY(QList<qreal> arg);

public slots:

protected slots:
    void updateData();

signals:
    void startXChanged(QList<qreal> arg);
    void endXChanged(QList<qreal> arg);
    void startYChanged(QList<qreal> arg);
    void endYChanged(QList<qreal> arg);

private:
    QList<qreal> m_startX;
    QList<qreal> m_endX;
    QList<qreal> m_startY;
    QList<qreal> m_endY;

};

#endif // LINESEGMENTPLOT_H
