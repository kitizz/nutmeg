#include "linesegmentplot.h"

#include "../../util/ndarray.h"
#include "../../util/arrayutil.h"

LineSegmentPlot::LineSegmentPlot(QQuickItem *parent)
    : LinePlot(parent)
    , m_dataSize(0)
{
    QMap<QString, QString> props;
    props["startX"] = "xData";
    props["startY"] = "yData";
    props["endX"] = "endX";
    props["endY"] = "endY";
    registerProperties(props);

//    connect(this, &LineSegmentPlot::startXChanged, this, &LineSegmentPlot::updateData);
//    connect(this, &LineSegmentPlot::startYChanged, this, &LineSegmentPlot::updateData);
//    connect(this, &LineSegmentPlot::endXChanged, this, &LineSegmentPlot::updateData);
//    connect(this, &LineSegmentPlot::endYChanged, this, &LineSegmentPlot::updateData);
}

LineSegmentPlot::~LineSegmentPlot()
{

}

NDArray &LineSegmentPlot::endX()
{
    return m_endX;
}

void LineSegmentPlot::setEndX(const NDArray &arg)
{
    m_endX = arg;
    emit endXChanged(arg);

    QRectF lim = dataLimits();
    updateDataLimits();
    if (lim == dataLimits()) {
        // Data limits not changed, trigger update manually
        triggerUpdate();
    }
}

NDArray &LineSegmentPlot::endY()
{
    return m_endY;
}

void LineSegmentPlot::setEndY(const NDArray &arg)
{
    m_endY = arg;
    emit endYChanged(arg);

    QRectF lim = dataLimits();
    updateDataLimits();
    if (lim == dataLimits()) {
        // Data limits not changed, trigger update manually
        triggerUpdate();
    }
}

int LineSegmentPlot::dataSize() const
{
    return m_dataSize;
}

void LineSegmentPlot::updateDataLimits()
{
    qDebug() << Q_FUNC_INFO;
    int xLen = xSize(), yLen = ySize(), endXLen = m_endX.size(), endYLen = m_endY.size();
    int N = qMin( qMin(xLen, yLen), qMin(endXLen, endYLen) );
    m_dataSize = N;

    qreal minX = Inf, maxX = -Inf, minY = Inf, maxY = -Inf;
    // Calculate the y limits
    RangeValues yrng = ArrayUtil::limits(yData());
    minY = yrng.min;
    maxY = yrng.max;

    yrng = ArrayUtil::limits(m_endY);
    minY = qMin(minY, yrng.min);
    maxY = qMax(maxY, yrng.max);

    RangeValues xrng = ArrayUtil::limits(xData());
    minX = xrng.min;
    maxX = xrng.max;

    xrng = ArrayUtil::limits(m_endX);
    minX = qMin(minX, xrng.min);
    maxX = qMax(maxX, xrng.max);

    QRectF newLim;
    newLim.setLeft(minX);
    newLim.setTop(minY);
    newLim.setRight(maxX);
    newLim.setBottom(maxY);
    setDataLimits(newLim);
}

