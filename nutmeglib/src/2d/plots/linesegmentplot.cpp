#include "linesegmentplot.h"

LineSegmentPlot::LineSegmentPlot(QQuickItem *parent)
    : LinePlot(parent)
{
    QMap<QString, QString> props;
    props["startX"] = "startX";
    props["startY"] = "startY";
    props["endX"] = "endX";
    props["endY"] = "endY";
    registerProperties(props);

    connect(this, &LineSegmentPlot::startXChanged, this, &LineSegmentPlot::updateData);
    connect(this, &LineSegmentPlot::startYChanged, this, &LineSegmentPlot::updateData);
    connect(this, &LineSegmentPlot::endXChanged, this, &LineSegmentPlot::updateData);
    connect(this, &LineSegmentPlot::endYChanged, this, &LineSegmentPlot::updateData);
}

LineSegmentPlot::~LineSegmentPlot()
{

}

QList<qreal> LineSegmentPlot::startX() const
{
    return m_startX;
}

void LineSegmentPlot::setStartX(QList<qreal> arg)
{
    if (m_startX == arg)
        return;

    m_startX = arg;
    emit startXChanged(arg);
}

QList<qreal> LineSegmentPlot::endX() const
{
    return m_endX;
}

void LineSegmentPlot::setEndX(QList<qreal> arg)
{
    if (m_endX == arg)
        return;

    m_endX = arg;
    emit endXChanged(arg);
}

QList<qreal> LineSegmentPlot::startY() const
{
    return m_startY;
}

void LineSegmentPlot::setStartY(QList<qreal> arg)
{
    if (m_startY == arg)
        return;

    m_startY = arg;
    emit startYChanged(arg);
}

QList<qreal> LineSegmentPlot::endY() const
{
    return m_endY;
}

void LineSegmentPlot::setEndY(QList<qreal> arg)
{
    if (m_endY == arg)
        return;

    m_endY = arg;
    emit endYChanged(arg);
}

void LineSegmentPlot::updateData()
{
//    QList<qreal> xD, yD;
//    xD.append(m_startX);
//    xD.append(m_endX);

//    yD.append(m_startY);
//    yD.append(m_endY);

//    setXData(xD);
//    setYData(yD);
}

