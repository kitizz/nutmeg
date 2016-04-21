#include "streamingplot.h"

#include "../../util/arrayutil.h"

StreamingPlot::StreamingPlot(QQuickItem *parent)
    : LinePlot(parent)
    , m_size(100)
    , m_datacount(0)
    , m_startx(0)
    , m_starty(0)
{
    XYPlot::setYData(NDArray(NDArray::Float64, {100}));
    registerMethod("appendX");
    registerMethod("extendX");
    registerMethod("appendY");
    registerMethod("extendY");
    registerMethod("appendXY");
    registerMethod("extendXY");
}

void StreamingPlot::appendXY(qreal x, qreal y)
{
    appendX(x);
    appendY(y);
}

void StreamingPlot::extendXY(QVariantList x, QVariantList y)
{
    extendX(x);
    extendY(y);
}

void StreamingPlot::appendX(qreal x)
{
    if (m_datacount == m_size) {
        XYPlot::xData().at<qreal>(m_startx) = x;
        m_startx = (m_startx + 1) % m_size;
    } else {
        int ind = (m_startx + m_datacount) % m_size;
        XYPlot::xData().at<qreal>(ind) = x;
        ++m_datacount;
    }
    updateDataLimits();
}

void StreamingPlot::extendX(QVariantList xs)
{
    foreach (auto var, xs) {
        appendX(var.toReal());
    }
}

void StreamingPlot::appendY(qreal y)
{
    qDebug() << "AppendY" << y;
    qDebug() << "Datacount, size" << m_datacount << m_size;
    qDebug() << "StartY:" << m_starty;
    if (m_datacount == m_size) {
        qDebug() << "Placing data at:" << m_starty;
        XYPlot::yData().at<qreal>(m_starty) = y;
        m_starty = (m_starty + 1) % m_size;
    } else {
        int ind = (m_starty + m_datacount) % m_size;
        qDebug() << "Placing data at:" << ind;
        XYPlot::yData().at<qreal>(ind) = y;
        ++m_datacount;
    }
    updateDataLimits();
    triggerUpdate();
}

void StreamingPlot::extendY(QVariantList ys)
{
    foreach (auto var, ys) {
        appendY(var.toReal());
    }
}

int StreamingPlot::size() const
{
    return m_size;
}

int StreamingPlot::xSize() const
{
    return qMin(m_datacount, XYPlot::xSize());
}

int StreamingPlot::ySize() const
{
    return qMin(m_datacount, XYPlot::ySize());
}

void StreamingPlot::setXData(const NDArray &arg)
{
    m_datacount = qMax(arg.size(), ySize());
    m_startx = 0;

    if (arg.size() < m_size) {
        // Copy into a properly sized buffer
        NDArray newarray = ArrayUtil::resize1d(arg, m_size);
        XYPlot::setXData(newarray);

    } else {
        // This is fine, keep it
        XYPlot::setXData(arg);
    }
}

void StreamingPlot::setYData(const NDArray &arg)
{
    m_datacount = qMax(arg.size(), xSize());
    m_starty = 0;

    if (arg.size() < m_size) {
        // Copy into a properly sized buffer
        NDArray newarray = ArrayUtil::resize1d(arg, m_size);
        XYPlot::setYData(newarray);

    } else {
        // This is fine, keep it
        XYPlot::setYData(arg);
    }
}

int StreamingPlot::startIndexX() const
{
    return m_startx;
}

int StreamingPlot::startIndexY() const
{
    return m_starty;
}

void StreamingPlot::updateDataLimits()
{
    qDebug() << Q_FUNC_INFO;
    int xLen = xSize(), yLen = ySize();
    int N = xLen > 0 ? qMin(xLen, yLen) : yLen;
    if (m_settingData)
        return; // Don't update until the data is good

    qreal minX = Inf, maxX = -Inf, minY = Inf, maxY = -Inf;

    if (N > 0) {
        // Calculate the y limits
        RangeValues yrng = ArrayUtil::limitsWrap(yData(), m_starty, N);
        minY = yrng.min;
        maxY = yrng.max;

        // Calculate the X bounds (special case)
        if (xLen > 0) {
            RangeValues xrng = ArrayUtil::limitsWrap(xData(), m_startx, N);
            minX = xrng.min;
            maxX = xrng.max;
        } else {
            minX = 0;
            maxX = yLen;
        }
    }

    qDebug() << "Xlen, Ylen" << xLen << yLen;
    qDebug() << "Limits:" << minX << minY << maxX << maxY;

    QRectF newLim;
    newLim.setLeft(minX);
    newLim.setTop(minY);
    newLim.setRight(maxX);
    newLim.setBottom(maxY);
    setDataLimits(newLim);
}

/*!
 * \brief StreamingPlot::setSize
 * Set the sie of the FIFO buffer. NOTE: Currently, changing this after related append and extend
 * methods have been called will result in undefined behavior.
 * \param size
 */
void StreamingPlot::setSize(int size)
{
    if (m_size == size)
        return;

    // Resize arrays as needed
    if (size > XYPlot::xSize()) {
        NDArray newarray = ArrayUtil::resize1d(xData(), size);
        XYPlot::setXData(newarray);
    }
    if (size > XYPlot::ySize()) {
        NDArray newarray = ArrayUtil::resize1d(yData(), size);
        XYPlot::setYData(newarray);
    }
    // Updated the data count of size is now smaller
    if (size < m_datacount) {
        m_datacount = size;
    }

    m_size = size;
    emit sizeChanged(size);
}
