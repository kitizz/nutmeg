#include "xyplot.h"

#include "../../util/util.h"

#include "../../util/ndarray.h"
#include "../../util/arrayutil.h"

XYPlot::XYPlot(QQuickItem *parent)
    : Plot2DBase(parent)
    , m_settingData(false)
    , m_xDataSet(false)
    , m_xData(NDArray())
    , m_yData(NDArray())
//    , m_tree(0)
    , m_treeDirty(false)
{
    connect(this, &XYPlot::xDataChanged, this, &PlotBase::dataChanged);
    connect(this, &XYPlot::yDataChanged, this, &PlotBase::dataChanged);
}

int XYPlot::nearestDataTo(const QPointF &dataLocation)
{
//    updateTree();
//    if (!m_tree)
//        return -1;

//    QKDTreeNode node;
//    qDebug() << "Tree size:" << m_tree->size();
//    bool success = m_tree->nearestNode(dataLocation, &node);

//    if (success) {
//        return node.value().toInt();
//    } else {
//        return -1;
//    }
    return -1;
}


NDArray &XYPlot::xData()
{
    return m_xData;
}

void XYPlot::setXData(const NDArray &arg)
{
//    qDebug() << Q_FUNC_INFO << arg.shape();
    m_xData = arg;
    m_xDataSet = m_xData.size() > 0;
    m_treeDirty = true;
    emit xDataChanged(arg);

    QRectF lim = dataLimits();
    updateDataLimits();
    if (lim == dataLimits()) {
        // Data limits not changed, trigger update manually
        triggerUpdate();
    }
}

NDArray &XYPlot::yData()
{
    return m_yData;
}

void XYPlot::setYData(const NDArray &arg)
{
    // Throw caution to the wind, too slow to check each time if it's equal
    m_yData = arg;
    m_treeDirty = true;
    emit yDataChanged(arg);

    QRectF lim = dataLimits();
    updateDataLimits();
    if (lim == dataLimits()) {
        // Data limits not changed, trigger update manually
        triggerUpdate();
    }
}

int XYPlot::xSize() const
{
    return m_xData.size();
}

int XYPlot::ySize() const
{
    return m_yData.size();
}

int XYPlot::startIndexX() const
{
    return 0;
}

int XYPlot::startIndexY() const
{
    return 0;
}

/*!
 * \brief XYPlot::dataLocationAt
 * \param index Index of data point
 * \return The location of the data point in data coordinates
 */
QPointF XYPlot::dataLocationAt(int index)
{
    if (index < 0 || index >= m_yData.size())
        return QPointF(Inf, Inf);
    return QPointF(m_xData.at<qreal>(index), m_yData.at<qreal>(index));
}

/*!
 * \brief XYPlot::frameLocationAt
 * \param index Index of data point
 * \return The location of the data point in item coordinates
 */
QPointF XYPlot::frameLocationAt(int index)
{
    return dataToItem(dataLocationAt(index));
}

void XYPlot::setData(const NDArray &xVals, const NDArray &yVals)
{
    m_settingData = true;
    m_xData = xVals;
    m_xDataSet = xVals.size() > 0;
    m_yData = yVals;
    m_treeDirty = true;
    emit xDataChanged(xVals);
    emit yDataChanged(yVals);
    m_settingData = false;

    updateDataLimits();
}

void XYPlot::updateDataLimits()
{
    qDebug() << Q_FUNC_INFO;
    int xLen = xSize(), yLen = ySize();
    int N = qMin(xLen, yLen);
    if (m_settingData)
        return; // Don't update until the data is good

    qreal minX = Inf, maxX = -Inf, minY = Inf, maxY = -Inf;
    // Calculate the y limits
    RangeValues yrng = ArrayUtil::limits(m_yData);
    minY = yrng.min;
    maxY = yrng.max;

    // Calculate the X bounds (special case)
    if (xLen > 0) {
        RangeValues xrng = ArrayUtil::limits(m_xData);
        minX = xrng.min;
        maxX = xrng.max;
    } else {
        minX = 0;
        maxX = yLen;
    }

    QRectF newLim;
    newLim.setLeft(minX);
    newLim.setTop(minY);
    newLim.setRight(maxX);
    newLim.setBottom(maxY);
    setDataLimits(newLim);
}

void XYPlot::updateTree()
{
//    if (!m_treeDirty) return;

//    // Start from scratch
//    if (m_tree)
//        delete m_tree;
//    m_tree = new QKDTree(2);

//    m_treeDirty = false;

//    int N = m_yData.length();
//    if (N == 0) return;
//    QList<int> *inds = 0;
//    if (!m_xDataSet || dataIncreasing(m_xData)) {
//        // Check if the x values are always increasing
//        inds = Util::medianArray(N);
//    } else {
//        // Throwing in random indices is as good a method as any other...
//        inds = Util::randomArray(N);
//    }

//    foreach (int i, *inds) {
//        // Check for NaN
//        if (m_xData[i] != m_xData[i]) continue;
//        if (m_yData[i] != m_yData[i]) continue;
//        m_tree->add(QPointF(m_xData[i], m_yData[i]), i);
//    }

//    delete inds; // Clean up
}
