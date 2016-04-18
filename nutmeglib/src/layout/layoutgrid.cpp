#include "layoutgrid.h"

#include "../util/util.h"

LayoutGrid::LayoutGrid(QQuickItem *parent)
    : QQuickItem(parent)
    , m_axes(QList<AxisBase*>())
    , m_handle("")
    , m_rows(1)
    , m_columns(1)
    , m_layoutDirection(Horizontal)
    , m_reversed(false)
    , m_vspace(0)
    , m_hspace(0)
    , m_rowWeights(QList<qreal>())
//    , m_rowMinimums(QList<qreal>())
//    , m_rowMaximums(QList<qreal>())
    , m_columnWeights(QList<qreal>())
//    , m_columnMinimums(QList<qreal>())
//    , m_columnMaximums(QList<qreal>())
    , m_minXs(QVector<qreal>())
    , m_maxXs(QVector<qreal>())
    , m_minYs(QVector<qreal>())
    , m_maxYs(QVector<qreal>())
    , m_plotRectsDirty(true)
{
    connect(this, &QQuickItem::childrenChanged, this, &LayoutGrid::updateAxes);

    connect(this, &LayoutGrid::axesChanged, [=]{ updateLayout(); });
    connect(this, &LayoutGrid::layoutDirectionChanged, [=]{ updateLayout(); });
    connect(this, &LayoutGrid::reversedChanged, [=]{ updateLayout(); });
    connect(this, &LayoutGrid::rowsChanged, [=]{ updateLayout(); });
    connect(this, &LayoutGrid::columnsChanged, [=]{ updateLayout(); });
    connect(this, &LayoutGrid::vspaceChanged, [=]{ updateLayout(); });
    connect(this, &LayoutGrid::hspaceChanged, [=]{ updateLayout(); });

    connect(this, &LayoutGrid::rowWeightsChanged, [=]{ updateLayout(); });
    connect(this, &LayoutGrid::columnWeightsChanged, [=]{ updateLayout(); });

    connect(this, &QQuickItem::widthChanged, [=]{ updateLayout(); });
    connect(this, &QQuickItem::heightChanged, [=]{ updateLayout(); });

    setObjectName("NutmegGrid");
}

LayoutGrid::~LayoutGrid()
{

}

QString LayoutGrid::handle() const
{
    return m_handle;
}

void LayoutGrid::setHandle(QString arg)
{
    if (m_handle == arg)
        return;

    m_handle = arg;
    emit handleChanged(arg);
}

int LayoutGrid::rows() const
{
    return m_rows;
}

int LayoutGrid::columns() const
{
    return m_columns;
}

void LayoutGrid::setRows(int arg)
{
    if (m_rows == arg || m_layoutDirection == Horizontal)
        return;

    m_rows = arg;
    emit rowsChanged(arg);
}

void LayoutGrid::setColumns(int arg)
{
    if (m_columns == arg || m_layoutDirection == Vertical)
        return;

    m_columns = arg;
    emit columnsChanged(arg);
}

QList<AxisBase *> LayoutGrid::axes() const
{
    return m_axes;
}

//QQmlListProperty<AxisBase> LayoutGrid::axes()
//{
//    return QQmlListProperty<AxisBase>(this, 0, &appendAxis, &axisCount, &axis, &axesClear);
//}

//void LayoutGrid::appendAxis(QQmlListProperty<AxisBase> *list, AxisBase *ax)
//{
//    LayoutGrid *grid = qobject_cast<LayoutGrid*>(list->object);
//    if (grid && ax) {
//        grid->m_axes.append(ax);
//        ax->setLayoutGroup(grid);
//        emit grid->axesChanged();
//    }
//}

//void LayoutGrid::axesClear(QQmlListProperty<AxisBase> *list)
//{
//    LayoutGrid *grid = qobject_cast<LayoutGrid*>(list->object);
//    if (grid && grid->m_axes.count()) {
//        foreach (AxisBase *ax, grid->m_axes)
//            ax->setLayoutGroup(0);
//        grid->m_axes.clear();
//        emit grid->axesChanged();
//    }
//}

//int LayoutGrid::axisCount(QQmlListProperty<AxisBase> *list)
//{
//    LayoutGrid *grid = qobject_cast<LayoutGrid*>(list->object);
//    if (grid)
//        return grid->m_axes.count();
//    return 0;
//}

//AxisBase *LayoutGrid::axis(QQmlListProperty<AxisBase> *list, int index)
//{
//    LayoutGrid *grid = qobject_cast<LayoutGrid*>(list->object);
//    if (grid)
//        return grid->m_axes[index];
//    return 0;
//}

//AxisLayoutAttached *LayoutGrid::qmlAttachedProperties(QObject *obj)
//{
//    return new AxisLayoutAttached(obj);
//}

LayoutGrid::LayoutDirection LayoutGrid::layoutDirection() const
{
    return m_layoutDirection;
}

void LayoutGrid::setLayoutDirection(LayoutGrid::LayoutDirection arg)
{
    if (m_layoutDirection == arg)
        return;

    m_layoutDirection = arg;
    emit layoutDirectionChanged(arg);
}

bool LayoutGrid::reversed() const
{
    return m_reversed;
}

void LayoutGrid::setReversed(bool arg)
{
    if (m_reversed == arg)
        return;

    m_reversed = arg;
    emit reversedChanged(arg);
}

QList<qreal> LayoutGrid::rowWeights() const
{
    return m_rowWeights;
}

void LayoutGrid::setRowWeights(QList<qreal> arg)
{
    if (m_rowWeights == arg)
        return;

    m_rowWeights = arg;
    emit rowWeightsChanged(arg);
}

//QList<qreal> LayoutGrid::rowMinimums() const
//{
//    return m_rowMinimums;
//}

//void LayoutGrid::setRowMinimums(QList<qreal> arg)
//{
//    if (m_rowMinimums == arg)
//        return;

//    m_rowMinimums = arg;
//    emit rowMinimumsChanged(arg);
//}

//QList<qreal> LayoutGrid::rowMaximums() const
//{
//    return m_rowMaximums;
//}

//void LayoutGrid::setRowMaximums(QList<qreal> arg)
//{
//    if (m_rowMaximums == arg)
//        return;

//    m_rowMaximums = arg;
//    emit rowMaximumsChanged(arg);
//}

QList<qreal> LayoutGrid::columnWeights() const
{
    return m_columnWeights;
}

void LayoutGrid::setColumnWeights(QList<qreal> arg)
{
    if (m_columnWeights == arg)
        return;

    m_columnWeights = arg;
    emit columnWeightsChanged(arg);
}

void LayoutGrid::setPlotRectsDirty()
{
    m_plotRectsDirty = true;
}

//QList<qreal> LayoutGrid::columnMinimums() const
//{
//    return m_columnMinimums;
//}

//void LayoutGrid::setColumnMinimums(QList<qreal> arg)
//{
//    if (m_columnMinimums == arg)
//        return;

//    m_columnMinimums = arg;
//    emit columnMinimumsChanged(arg);
//}

//QList<qreal> LayoutGrid::columnMaximums() const
//{
//    return m_columnMaximums;
//}

//void LayoutGrid::setColumnMaximums(QList<qreal> arg)
//{
//    if (m_columnMaximums == arg)
//        return;

//    m_columnMaximums = arg;
//    emit columnMaximumsChanged(arg);
//}

void LayoutGrid::updateAxes()
{
    QList<AxisBase*> newAxes;
    bool changed = false;

    int i = 0;
    foreach (QObject* child, children()) {
        AxisBase *ax = qobject_cast<AxisBase*>(child);
        if (!ax)
            continue;
        if (i + 1 >= m_axes.length() || m_axes[i] != ax)
            changed = true;

        ax->setLayoutGroup(this);
        newAxes << ax;
    }

    if (changed) {
        m_axes = newAxes;
        emit axesChanged();
    }
}

AxisBase *LayoutGrid::axisAt(const int row, const int col)
{
    return m_axes.value(plotIndex(row, col), 0);
}

void LayoutGrid::updateLayout()
{
    // Calculate the new row or column size
    const int N = m_axes.count();
    if (N == 0)
        return;

    if (m_layoutDirection == Horizontal) {
        int newRow = (N - 1)/m_columns + 1;
        if (newRow != m_rows) {
            m_rows = newRow;
            emit rowsChanged(m_rows);
        }
    } else {
        int newCol = (N - 1)/m_rows + 1;
        if (newCol != m_columns) {
            m_columns = newCol;
            emit columnsChanged(m_columns);
        }
    }

    // Work out heights and widths
    QVector<qreal> widths = distributeWeights(m_columns, m_columnWeights, width());
    QVector<qreal> heights = distributeWeights(m_rows, m_rowWeights, height());
    QVector<qreal> xs = cumSum(widths);
    QVector<qreal> ys = cumSum(heights);

    for (int c=0; c<m_columns; ++c) {
        for (int r=0; r<m_rows; ++r) {
            AxisBase *ax = axisAt(r, c);
            if (!ax) continue;
            ax->setX(xs[c] + c*m_hspace);
            ax->setY(ys[r] + r*m_vspace);
            ax->setWidth(widths[c]);
            ax->setHeight(heights[r]);
        }
    }
}

void LayoutGrid::updatePlotRects()
{
    if (!m_plotRectsDirty)
        return;

    QVector<qreal> minXs(m_columns, -Inf), maxXs(m_columns, Inf);
    QVector<qreal> minYs(m_rows, -Inf), maxYs(m_rows, Inf);

    for (int c=0; c<m_columns; ++c) {
        for (int r=0; r<m_rows; ++r) {
            QRectF rect = axisAt(r, c)->preferredPlotRect();
            minXs[c] = qMax(minXs[c], rect.left());
            maxXs[c] = qMin(maxXs[c], rect.right());
            minYs[r] = qMax(minYs[r], rect.top());
            maxYs[r] = qMin(maxYs[r], rect.bottom());
        }
    }

    m_minXs = minXs;
    m_maxXs = maxXs;
    m_minYs = minYs;
    m_maxYs = maxYs;

    m_plotRectsDirty = false;

    // TODO: Check how slow it is to update like this...
    for (int c=0; c<m_columns; ++c) {
        for (int r=0; r<m_rows; ++r) {
            QRectF rect = getPlotRectFor(r, c);
//            qDebug() << "PlotRect(" << r << c << "):" << rect;
            axisAt(r, c)->setPlotRect(rect);
        }
    }
}

QRectF LayoutGrid::getPlotRectFor(const int row, const int col)
{
    updatePlotRects();
    const qreal minX = m_minXs[col], maxX = m_maxXs[col];
    const qreal minY = m_minYs[row], maxY = m_maxYs[row];
    return QRectF(minX, minY, maxX - minX, maxY - minY);
}

qreal LayoutGrid::vspace() const
{
    return m_vspace;
}

void LayoutGrid::setVspace(qreal vspace)
{
    if (m_vspace == vspace)
        return;

    m_vspace = vspace;
    emit vspaceChanged(vspace);
}

qreal LayoutGrid::hspace() const
{
    return m_hspace;
}

void LayoutGrid::setHspace(qreal hspace)
{
    if (m_hspace == hspace)
        return;

    m_hspace = hspace;
    emit hspaceChanged(hspace);
}

// Not quite technically a normal cumSum. First value is zero and excludes final sum.
QVector<qreal> LayoutGrid::cumSum(QVector<qreal> values)
{
    QVector<qreal> result(values.count());
    qreal sum = 0;
    for(int i = 0; i < values.count(); ++i) {
        result[i] = sum;
        sum += values[i];
    }

    return result;
}

QVector<qreal> LayoutGrid::distributeWeights(const int N, const QList<qreal> weights, const qreal size)
{
    // TODO: Unit tests
//    const int N = weights.count();
    QVector<qreal> values(N);
    QList<int> free_weights;  // Keep track of unweighted sections
    qreal remaining = size;
    for (int n=0; n<N; ++n) {
        qreal v = n < weights.count() ? weights[n]*size : -1;
        if (v >= 0) {
            values[n] = v;
            remaining -= v;
        } else {
            free_weights << n;
        }
    }

    // Evenly distribute the remaining space to the free weights
    foreach (int n, free_weights)
        values[n] = remaining/free_weights.count();

    return values;
}

//FigureBase *LayoutGrid::getFigure()
//{
//    FigureBase *fig = qobject_cast<FigureBase*>(parent());
//    if (!fig && m_axes.count() > 0)
//        fig = m_axes[0]->figure();
//    return fig;
//}

//AxisLayoutAttached::AxisLayoutAttached(QObject *parent)
//    : QObject(parent)
//    , m_heightRatio(-1)
//    , m_minHeight(-1)
//    , m_maxHeight(-1)
//    , m_widthRatio(-1)
//    , m_minWidth(-1)
//    , m_maxWidth(-1)
//{

//}

//qreal AxisLayoutAttached::heightRatio() const
//{
//    return m_heightRatio;
//}

//qreal AxisLayoutAttached::minHeight() const
//{
//    return m_minHeight;
//}

//qreal AxisLayoutAttached::maxHeight() const
//{
//    return m_maxHeight;
//}

//qreal AxisLayoutAttached::widthRatio() const
//{
//    return m_widthRatio;
//}

//qreal AxisLayoutAttached::minWidth() const
//{
//    return m_minWidth;
//}

//qreal AxisLayoutAttached::maxWidth() const
//{
//    return m_maxWidth;
//}

//void AxisLayoutAttached::setHeightRatio(qreal arg)
//{
//    if (m_heightRatio == arg)
//        return;

//    m_heightRatio = arg;
//    emit heightRatioChanged(arg);
//}

//void AxisLayoutAttached::setMinHeight(qreal arg)
//{
//    if (m_minHeight == arg)
//        return;

//    m_minHeight = arg;
//    emit minHeightChanged(arg);
//}

//void AxisLayoutAttached::setMaxHeight(qreal arg)
//{
//    if (m_maxHeight == arg)
//        return;

//    m_maxHeight = arg;
//    emit maxHeightChanged(arg);
//}

//void AxisLayoutAttached::setWidthRatio(qreal arg)
//{
//    if (m_widthRatio == arg)
//        return;

//    m_widthRatio = arg;
//    emit widthRatioChanged(arg);
//}

//void AxisLayoutAttached::setMinWidth(qreal arg)
//{
//    if (m_minWidth == arg)
//        return;

//    m_minWidth = arg;
//    emit minWidthChanged(arg);
//}

//void AxisLayoutAttached::setMaxWidth(qreal arg)
//{
//    if (m_maxWidth == arg)
//        return;

//    m_maxWidth = arg;
//    emit maxWidthChanged(arg);
//}
