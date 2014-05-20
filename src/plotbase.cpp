#include "plotbase.h"
#include "axisbase.h"
#include <QPolygonF>
#include <QPainter>

#include <algorithm>

#include "../QKDTree/QVectorND/QVectorND.h"

PlotBase::PlotBase(QQuickItem *parent)
    : QQuickItem(parent)
    , m_settingData(false)
    , m_handle(QString())
    , m_xDataSet(false)
    , m_xData(QList<qreal>())
    , m_yData(QList<qreal>())
    , m_axis(0)
    , m_tree(0)
    , m_treeDirty(false)
    , m_dataLimits(QRectF())
{
    connect(this, &QQuickItem::parentChanged, this, &PlotBase::updateAxis);
}

AxisBase *PlotBase::axis() const
{
    return m_axis;
}

QString PlotBase::handle() const
{
    return m_handle;
}

void PlotBase::setHandle(QString arg)
{
    if (m_handle == arg) return;
    m_handle = arg;
    emit handleChanged(arg);
}

QQuickPaintedItem *PlotBase::canvas() const
{
    return m_canvas;
}

void PlotBase::setCanvas(QQuickPaintedItem *arg)
{
    if (m_canvas == arg) return;
    m_canvas = arg;
    emit canvasChanged(arg);
}

QString PlotBase::map(QString prop)
{
    return NutmegObject::map(prop);
}

QPointF PlotBase::itemToData(QPointF point)
{
    if (!m_axis) return point;
    qreal px = point.x(), py = height() - point.y();

    QRectF lim = m_axis->limits();
    qreal sx = lim.width() / width();
    qreal sy = lim.height() / height();
    if (width() == 0 || height() == 0) {
        qWarning() << "Zero width or height" << width() << height();
    }
    return QPointF(sx*px + lim.left(), sy*py + lim.top());
}

QList<QPointF> PlotBase::itemToDataList(QList<QPointF> points)
{
    // TODO: Research using Armadillo to do transformations...
    if (!m_axis) return points;

    QList<QPointF> newPoints;
    foreach (QPointF p, points) {
        qreal x = p.x(), y = height() - p.y();

        QRectF lim = m_axis->limits();
        qreal sx = lim.width() / width();
        qreal sy = lim.height() / height();

        newPoints << QPointF(sx*x + lim.left(), sy*y + lim.top());
    }
    return newPoints;
}

QPointF PlotBase::dataToItem(QPointF point)
{
    if (!m_axis) return point;
    qreal x = point.x(), y = point.y();

    QRectF lim = m_axis->limits();
    qreal sx = 1;
    qreal sy = 1;
    if (lim.width() != 0)
        sx = width() / lim.width();
    if (lim.height() != 0)
        sy = height() / lim.height();

    return QPointF( (x - lim.left())*sx, height() - (y - lim.top())*sy );
}

QList<QPointF> PlotBase::dataToItemList(QList<QPointF> points)
{
    // TODO: Research using Armadillo to do transformations...
    if (!m_axis) return points;

    QList<QPointF> newPoints;
    foreach (QPointF p, points) {
        qreal x = p.x(), y = p.y();

        QRectF lim = m_axis->limits();
        qreal sx = width() / lim.width();
        qreal sy = height() / lim.height();

        newPoints << QPointF( (x - lim.left())*sx, height() - (y - lim.top())*sy );
    }
    return newPoints;
}

int PlotBase::nearestDataTo(const QPointF &dataLocation)
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

void PlotBase::setAxis(AxisBase *arg)
{
    if (m_axis == arg) return;

    if (m_axis) {
        m_axis->disconnect(this);
        m_axis->deregisterPlot(this);
    }

    m_axis = arg;

    if (m_axis) {
        connect(m_axis, SIGNAL(minXChanged(qreal)), this, SLOT(triggerUpdate()));
        connect(m_axis, SIGNAL(minYChanged(qreal)), this, SLOT(triggerUpdate()));
        connect(m_axis, SIGNAL(maxXChanged(qreal)), this, SLOT(triggerUpdate()));
        connect(m_axis, SIGNAL(maxYChanged(qreal)), this, SLOT(triggerUpdate()));
        connect(m_axis, SIGNAL(dataLimitsChanged(QRectF)), this, SLOT(triggerUpdate()));
        m_axis->registerPlot(this);
    } else {
        qWarning() << Q_FUNC_INFO << "Plot is not a descendant of any Axis. It may not behave as expected.";
    }
    emit axisChanged(arg);
}

void PlotBase::triggerUpdate()
{
    update();
    if (m_canvas)
        m_canvas->update();
}

void PlotBase::updateAxis()
{
    // Work up the tree until the next Axis item is found.
    QObject *newParent = parent();
    AxisBase *axis;
    while (newParent) {
        axis = qobject_cast<AxisBase*>(newParent);
        if (axis) break;
        newParent = newParent->parent();
    }

    setAxis(axis);
}

QList<qreal> PlotBase::xData() const
{
    return m_xData;
}

void PlotBase::setXData(QList<qreal> arg)
{
    if (m_xData == arg) return;
    m_xData = arg;
    m_xDataSet = m_xData.length() > 0;
    m_treeDirty = true;
    emit xDataChanged(arg);
    updateData();
}

QList<qreal> PlotBase::yData() const
{
    return m_yData;
}

void PlotBase::setYData(QList<qreal> arg)
{
    if (m_yData == arg) return;
    m_yData = arg;
    m_treeDirty = true;
    emit yDataChanged(arg);
    updateX();
    updateData();
}

/*!
 * \brief PlotBase::dataLocationAt
 * \param index Index of data point
 * \return The location of the data point in data coordinates
 */
QPointF PlotBase::dataLocationAt(int index)
{
    if (index < 0 || index >= m_yData.length())
        return QPointF(Inf, Inf);
    return QPointF(m_xData[index], m_yData[index]);
}

/*!
 * \brief PlotBase::frameLocationAt
 * \param index Index of data point
 * \return The location of the data point in item coordinates
 */
QPointF PlotBase::frameLocationAt(int index)
{
    return dataToItem(dataLocationAt(index));
}

void PlotBase::setData(QList<qreal> xVals, QList<qreal> yVals)
{
    m_settingData = true;
    m_xData = xVals;
    m_xDataSet = xVals.length() > 0;
    m_yData = yVals;
    m_treeDirty = true;
    emit xDataChanged(xVals);
    emit yDataChanged(yVals);
    m_settingData = false;

    updateData();
}

QRectF PlotBase::dataLimits() const
{
    return m_dataLimits;
}

void PlotBase::updateX()
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

void PlotBase::updateData()
{
    int xLen = m_xData.length(), yLen = m_yData.length();
    if ((xLen > 0 && xLen != yLen) || m_settingData)
        return; // Don't update until the data is good

//    qDebug() << "Updating data for" << handle();
    qreal minX = Inf, maxX = -Inf, minY = Inf, maxY = -Inf;
    for (int i=0; i<yLen; ++i) {
        qreal px = m_xData[i];
        qreal py = m_yData[i];
        minX = qMin(minX, px);
        maxX = qMax(maxX, px);
        minY = qMin(minY, py);
        maxY = qMax(maxY, py);
    }

//    qDebug() << "Datalimits:" << minX << maxX << minY << maxY;

    m_dataLimits.setLeft(minX);
    m_dataLimits.setTop(minY);
    m_dataLimits.setRight(maxX);
    m_dataLimits.setBottom(maxY);
    emit dataLimitsChanged(m_dataLimits);

    triggerUpdate();
}

void PlotBase::updateTree()
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
        inds = medianArray(N);
    } else {
        // Throwing in random indices is as good a method as any other...
        inds = randomArray(N);
    }

    foreach (int i, *inds) {
        // Check for NaN
        if (m_xData[i] != m_xData[i]) continue;
        if (m_yData[i] != m_yData[i]) continue;
        m_tree->add(QPointF(m_xData[i], m_yData[i]), i);
    }

    delete inds; // Clean up
}

int PlotBase::randInt(int low, int high) {
    // From: http://goo.gl/KdHeUg
    return qrand() % ((high + 1) - low) + low;
}

QList<int>* PlotBase::randomArray(int N)
{
    // http://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle
    QList<int> *lst = new QList<int>();
    lst->reserve(N);

    for (int i=0; i<N; ++i)
        (*lst)[i] = i;

    for (int i=0; i<N; ++i) {
        int n = randInt(i, N-1);
        lst->swap(i, n);
    }

    return lst;
}

QList<int>* PlotBase::medianArray(int N, int s, QList<int>* lst)
{
    if (!lst) {
        lst = new QList<int>();
        lst->reserve(N);
    }

    if (N - s <= 2) {
        lst->append(s);
        if (N - s == 2)
            lst->append(s + 1);
        return lst;
    }

    int i = (s + N - 1)/2;
    lst->append(i);

    medianArray(i, s, lst);
    medianArray(N, i + 1, lst);

    return lst;
}

bool PlotBase::dataIncreasing(QList<qreal> data)
{
    // Return true if each data point is >= to the previous.
    // TODO: If this is a performance hit, consider allowing approximation (check each 10th element or something).
    for (int i=0; i<data.length() - 1; ++i) {
        if (data[i+1] < data[i])
            return false;
    }
    return true;
}
