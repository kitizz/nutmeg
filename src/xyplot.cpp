#include "xyplot.h"

#include "util.h"

XYPlot::XYPlot(QQuickItem *parent)
    : PlotBase(parent)
    , m_settingData(false)
    , m_xDataSet(false)
    , m_xData(QList<qreal>())
    , m_yData(QList<qreal>())
    , m_tree(0)
    , m_treeDirty(false)
{
}

int XYPlot::nearestDataTo(const QPointF &dataLocation)
{
    updateTree();
    if (!m_tree)
        return -1;

    QKDTreeNode node;
    qDebug() << "Tree size:" << m_tree->size();
    bool success = m_tree->nearestNode(dataLocation, &node);

    if (success) {
        return node.value().toInt();
    } else {
        return -1;
    }
}


QList<qreal> XYPlot::xData() const
{
    return m_xData;
}

void XYPlot::setXData(QList<qreal> arg)
{
    if (m_xData == arg) return;
    m_xData = arg;
    m_xDataSet = m_xData.length() > 0;
    m_treeDirty = true;
    emit xDataChanged(arg);
    updateDataLimits();
}

QList<qreal> XYPlot::yData() const
{
    return m_yData;
}

void XYPlot::setYData(QList<qreal> arg)
{
    if (m_yData == arg) return;
    m_yData = arg;
    m_treeDirty = true;
    emit yDataChanged(arg);
    updateX();
    updateDataLimits();
}

/*!
 * \brief XYPlot::dataLocationAt
 * \param index Index of data point
 * \return The location of the data point in data coordinates
 */
QPointF XYPlot::dataLocationAt(int index)
{
    if (index < 0 || index >= m_yData.length())
        return QPointF(Inf, Inf);
    return QPointF(m_xData[index], m_yData[index]);
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

void XYPlot::setData(QList<qreal> xVals, QList<qreal> yVals)
{
    m_settingData = true;
    m_xData = xVals;
    m_xDataSet = xVals.length() > 0;
    m_yData = yVals;
    m_treeDirty = true;
    emit xDataChanged(xVals);
    emit yDataChanged(yVals);
    m_settingData = false;

    updateDataLimits();
}

void XYPlot::updateX()
{
    if (m_xDataSet) return;
    int N = m_xData.length();
    int M = m_yData.length();

    // Nothing to update
    if (M == N) return;
    // Clear the list, save time.
    if (M == 0) {
        m_xData.clear();
    }
    // Add to xData
    if (M > N) {
        for (int i=N; i < M; ++i)
            m_xData.append(i);
    }
    // Remove xData
    if (M < N) {
        if (N - M <= M) {
            // Quicker to pop off the end
            for (int i=0; i < N-M; ++i)
                m_xData.removeLast();
        } else {
            // Quicker to copy the first M elements over
            m_xData = m_xData.mid(0, M);
        }
    }

    m_treeDirty = true;
    emit xDataChanged(m_xData);
}

void XYPlot::updateDataLimits()
{
    int xLen = m_xData.length(), yLen = m_yData.length();
    if ((xLen > 0 && xLen != yLen) || m_settingData)
        return; // Don't update until the data is good

    qreal minX = Inf, maxX = -Inf, minY = Inf, maxY = -Inf;
    for (int i=0; i<yLen; ++i) {
        qreal px = m_xData[i];
        qreal py = m_yData[i];
        minX = qMin(minX, px);
        maxX = qMax(maxX, px);
        minY = qMin(minY, py);
        maxY = qMax(maxY, py);
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
    if (!m_treeDirty) return;

    // Start from scratch
    if (m_tree)
        delete m_tree;
    m_tree = new QKDTree(2);

    m_treeDirty = false;

    int N = m_yData.length();
    if (N == 0) return;
    QList<int> *inds = 0;
    if (!m_xDataSet || dataIncreasing(m_xData)) {
        // Check if the x values are always increasing
        inds = Util::medianArray(N);
    } else {
        // Throwing in random indices is as good a method as any other...
        inds = Util::randomArray(N);
    }

    foreach (int i, *inds) {
        // Check for NaN
        if (m_xData[i] != m_xData[i]) continue;
        if (m_yData[i] != m_yData[i]) continue;
        m_tree->add(QPointF(m_xData[i], m_yData[i]), i);
    }

    delete inds; // Clean up
}

bool XYPlot::dataIncreasing(QList<qreal> data)
{
    // Return true if each data point is >= to the previous.
    // TODO: If this is a performance hit, consider allowing approximation (check each 10th element or something).
    for (int i=0; i<data.length() - 1; ++i) {
        if (data[i+1] < data[i])
            return false;
    }
    return true;
}
