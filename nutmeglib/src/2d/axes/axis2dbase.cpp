#include "axis2dbase.h"
#include "../../util/util.h"
#include <math.h>
#include <QtCore/qmath.h>

#include "../../common/figurebase.h"

// TODO: Remove
#include <QTime>

Axis2DBase::Axis2DBase(QQuickItem *parent)
    : AxisBase(parent)
    , m_grid(new AxisGrid(parent))
    , m_xAxis(new AxisSpec(parent))
    , m_yAxis(new AxisSpec(parent))
    , m_minX(-Inf)
    , m_maxX(Inf)
    , m_minY(-Inf)
    , m_maxY(Inf)
    , m_minXFloat(true)
    , m_maxXFloat(true)
    , m_minYFloat(true)
    , m_maxYFloat(true)
    , m_settingLimits(false)
    , m_dataLimits(QRectF(0, 0, 1, 1))
    , m_yLimitRounding(QList<qreal>())
    , m_margin(new AxisMargins())
    , m_aspectRatio(0)
    , m_fitPlots(false)
    , m_canvas(0)
    , m_shareX("")
    , m_shareY("")
{
    // Set initial axis label roundings
    m_yLimitRounding << 0 << 1 << 1.5 << 2 << 2.5 << 3 << 4 << 5 << 10;
    // Initialise the limits to inf. This defaults the limits to the data.
    m_limits = QRectF();
    m_limits.setTop(-Inf);
    m_limits.setLeft(-Inf);
    m_limits.setBottom(Inf);
    m_limits.setRight(Inf);

    // Register properties available through the API
    QMap<QString,QString> props;
    props.insert("minX", "minX");
    props.insert("maxX", "maxX");
    props.insert("minY", "minY");
    props.insert("maxY", "maxY");
    props.insert("xAxis", "xAxis");
    props.insert("yAxis", "yAxis");
    props.insert("grid", "grid");
    props.insert("shareX", "shareX");
    props.insert("shareY", "shareY");
    registerProperties(props);
    connect(this, &QQuickItem::parentChanged, this, &Axis2DBase::updateFigure);

    // Connections for X axis lengths
    connect(this, &QQuickItem::widthChanged, this, &Axis2DBase::updateXAxis);
    connect(m_margin, &AxisMargins::leftChanged, this, &Axis2DBase::updateXAxis);
    connect(m_margin, &AxisMargins::rightChanged, this, &Axis2DBase::updateXAxis);
//    connect(this, &AxisBase::minXChanged, [=](qreal val){ m_xAxis->setMin(val); });
//    connect(this, &AxisBase::maxXChanged, [=](qreal val){ m_xAxis->setMax(val); });
    m_xAxis->setObjectName("XAxis");
    m_xAxis->setMin(minX());
    m_xAxis->setMax(maxX());
    m_xAxis->setPixelSize(width());

    // Connections for Y axis lengths
    connect(this, &QQuickItem::heightChanged, this, &Axis2DBase::updateYAxis);
    connect(m_margin, &AxisMargins::topChanged, this, &Axis2DBase::updateYAxis);
    connect(m_margin, &AxisMargins::bottomChanged, this, &Axis2DBase::updateYAxis);
//    connect(this, &AxisBase::minYChanged, [=](qreal val){ m_yAxis->setMin(val); });
//    connect(this, &AxisBase::maxYChanged, [=](qreal val){ m_yAxis->setMax(val); });
    m_yAxis->setObjectName("YAxis");
    m_yAxis->setMin(minY());
    m_yAxis->setMax(maxY());
    m_yAxis->setPixelSize(height());

    // Connections for limits
    connect(this, &Axis2DBase::minXChanged, this, &Axis2DBase::updateLimits);
    connect(this, &Axis2DBase::minYChanged, this, &Axis2DBase::updateLimits);
    connect(this, &Axis2DBase::maxXChanged, this, &Axis2DBase::updateLimits);
    connect(this, &Axis2DBase::maxYChanged, this, &Axis2DBase::updateLimits);
    connect(this, &Axis2DBase::dataLimitsChanged, this, &Axis2DBase::updateLimits);
    connect(this, &Axis2DBase::plotRectChanged, this, &Axis2DBase::updateLimits);
    connect(this, &QQuickItem::widthChanged, this, &Axis2DBase::updateLimits);
    connect(this, &QQuickItem::heightChanged, this, &Axis2DBase::updateLimits);

    // Connections for axis updates
//    connect(m_xAxis, &AxisSpec::ticksChanged, this, &AxisBase::xAxisChanged);
//    connect(m_yAxis, &AxisSpec::ticksChanged, this, &AxisBase::yAxisChanged);

    // Connections for plot updates
    connect(this, &AxisBase::addedPlot, [=](PlotBase* plot) {
        Plot2DBase *plot2d = qobject_cast<Plot2DBase*>(plot);
        if (!plot2d) return;
        connect(plot2d, &Plot2DBase::dataLimitsChanged, this, &Axis2DBase::updateDataLimits);
        connect(this, &Axis2DBase::limitsChanged, plot2d, &Plot2DBase::triggerUpdate);
        connect(this, &AxisBase::plotRectChanged, plot2d, &Plot2DBase::triggerUpdate);
        updateDataLimits();
    });
}

qreal Axis2DBase::log_10(qreal v)
{
    return log10(v);
}

/*! \internal
 * \fn AxisBase::formatReal
 * Given \a num, and \a precision, return a string representation of the number
 * where exponentials are used if the magnitude of the number is > 10^precision
 * or < 10^-precision.
 */
QString Axis2DBase::formatReal(qreal num, int precision, int minMag, int maxMag)
{
    QString str;
    qreal abNum = qAbs(num);

    if (maxMag < 0)
        maxMag = precision;

    if (abNum < qPow(10, minMag)) {
        str = "0";

    } else if (abNum > qPow(10, maxMag) || abNum < qPow(10, -maxMag)) {
        int exp = (int)qFloor(log10( abNum ));
        qreal base = num * qPow(10, -exp);
        str.sprintf("%.*ge%d", precision, base, exp);

    } else {
        str.sprintf("%.*g", precision, num);
    }

    return str;
}

qreal Axis2DBase::offsetFromStd(qreal val, qreal std)
{
    int precStd = (int)qFloor(log10( qAbs(std) ));
    return qRound64(val*qPow(10, -precStd))*qPow(10, precStd);
}

void Axis2DBase::print(QPainter *painter)
{
    if (!m_canvas)
        return;

    m_canvas->paint(painter);

    foreach (QString key, plotNames()) {
        PlotBase *plt = plot(key);
        plt->print(painter);
    }
}

void Axis2DBase::triggerRedraw()
{
    if (m_canvas) {
        m_canvas->update();
    }
}

/*!
 * \property Axis2DBase::minX
 * Set the minimim X data limit.
 * \sa AxisBase::limits
 */
qreal Axis2DBase::minX() const
{
    return m_minX;
}

void Axis2DBase::setMinX(qreal arg, bool fix, bool updateAxis)
{
    // "fix" is used internally so that the bounds can be set while
    // letting them continue to float...
    if (arg != arg) // NaN
        return;
    if (arg == -Inf) {
        arg = m_dataLimits.left();
        m_minXFloat = true;
    } else if (fix && m_minXFloat) {
        m_minXFloat = false;
    }
    if (m_minX == arg) return;

    m_minX = arg;
    emit minXChanged(arg);

    // "emitSig" gives control of emitting the changed signal to the caller.
    // This removes the need for the above boilerplate code in "setLimits(...)"
    if (!updateAxis) return;
    m_xAxis->setMin(arg);
}

/*!
 * \property Axis2DBase::maxX
 * Set the maximum X data limit.
 * \sa Axis2DBase::limits
 */
qreal Axis2DBase::maxX() const
{
    return m_maxX;
}

void Axis2DBase::setMaxX(qreal arg, bool fix, bool updateAxis)
{
    if (arg != arg) // NaN
        return;
    if (arg == Inf) {
        arg = m_dataLimits.right();
        m_maxXFloat = true;
    } else if (fix && m_maxXFloat) {
        m_maxXFloat = false;
    }
    if (m_maxX == arg) return;

    m_maxX = arg;
    emit maxXChanged(arg);

    if (!updateAxis) return;
    m_xAxis->setMax(arg);
}

/*!
 * \property Axis2DBase::minY
 * Set the minimim Y data limit.
 * \sa Axis2DBase::limits
 */
qreal Axis2DBase::minY() const
{
    return m_minY;
}

void Axis2DBase::setMinY(qreal arg, bool fix, bool updateAxis)
{
    if (arg != arg) // NaN
        return;
    if (arg == -Inf) {
        arg = m_dataLimits.top();
        m_minYFloat = true;
    } else if (fix && m_minYFloat) {
        m_minYFloat = false;
    }
    if (m_minY == arg) return;

    m_minY = arg;
    emit minYChanged(arg);

    if (!updateAxis) return;
    m_yAxis->setMin(arg);
}

/*!
 * \property Axis2DBase::maxY
 * Set the maximum Y data limit.
 * \sa Axis2DBase::limits
 */
qreal Axis2DBase::maxY() const
{
    return m_maxY;
}

void Axis2DBase::setMaxY(qreal arg, bool fix, bool updateAxis)
{
    if (arg != arg) // NaN
        return;
    if (arg == Inf) {
        arg = m_dataLimits.bottom();
        m_maxYFloat = true;
    } else if (fix && m_maxYFloat) {
        m_maxYFloat = false;
    }
    if (m_maxY == arg) return;

    m_maxY = arg;
    emit maxYChanged(arg);

    if (!updateAxis) return;
    m_xAxis->setMax(arg);
}

void Axis2DBase::offset(qreal x, qreal y)
{
    // TODO: Implement Axis2DBase::offset
}

QPointF Axis2DBase::itemToPlot(qreal x, qreal y)
{
    QRectF frame = plotRect();
    qreal frameX = x - frame.x();
    qreal frameY = y - frame.y();

    if (xAxis()->inverted())
        frameX = frame.width() - frameX;
    if (!yAxis()->inverted())  // Flip for screen -> cartesian
        frameY = frame.height() - frameY;

    qreal sx = (maxX() - minX()) / frame.width();
    qreal sy = (maxY() - minY()) / frame.height();

    return QPointF(frameX*sx + minX(), frameY*sy + minY());
}

QPointF Axis2DBase::plotToItem(qreal x, qreal y)
{
//    qDebug() << "Plot to Item" << x << y;
    qreal localX = x - minX();
    qreal localY = y - minY();
//    qDebug() << localX << localY;

    QRectF frame = plotRect();
    qreal sx = frame.width() / (maxX() - minX());
    qreal sy = frame.height() / (maxY() - minY());

    qreal frameX = sx*localX;
    qreal frameY = sy*localY;
    if (xAxis()->inverted())
        frameX = frame.width() - frameX;
    if (!yAxis()->inverted())  // Flip for screen -> cartesian
        frameY = frame.height() - frameY;

    return QPointF(frame.x() + frameX, frame.y() + frameY);
}

void Axis2DBase::updateXAxis()
{
    qreal w = width() - m_margin->left() - m_margin->right();
    m_xAxis->setPixelSize(w);
}

void Axis2DBase::updateYAxis()
{
    qreal h = height() - m_margin->top() - m_margin->bottom();
    m_yAxis->setPixelSize(h);
}

void Axis2DBase::updateLimits()
{
    if (m_settingLimits) return;
    QRectF newLimits;
    newLimits.setLeft(minX());
    newLimits.setTop(minY());
    newLimits.setRight(maxX());
    newLimits.setBottom(maxY());

    newLimits = maintainAspectRatio(newLimits);

//    qDebug() << "Axis2DBase::updateLimits" << minX() << maxX() << minY() << maxY();
    setLimits(newLimits, false);
}

void Axis2DBase::updateDataLimits()
{
    qDebug() << "Axis2DBase::updateDataLimits";
    qreal minX = Inf, maxX = -Inf, minY = Inf, maxY = -Inf;

    bool validLimits = false;
    foreach (QString key, plotNames()) {
        Plot2DBase* plt = qobject_cast<Plot2DBase*>(plot(key));
        if (!plt) continue;

        QRectF rect = plt->dataLimits();
        qDebug() << "Plot dataLimits:" << plt << rect;
        if (rect.isEmpty()) continue;
        validLimits = true;

        if (rect.left() < minX) minX = rect.left();
        if (rect.right() > maxX) maxX = rect.right();
        if (rect.top() < minY) minY = rect.top();
        if (rect.bottom() > maxY) maxY = rect.bottom();
    }

    if (!validLimits) {
        qDebug() << Q_FUNC_INFO << "Limits not valid...";
        m_dataLimits = QRect(0, 0, 1, 1);
        return;
    }

    // Finalise the data limits
    QRectF rect;
    rect.setLeft(minX);
    rect.setRight(maxX);
    rect.setTop(minY);
    rect.setBottom(maxY);
    if (m_dataLimits != rect) {
        m_dataLimits = rect;
        emit dataLimitsChanged(m_dataLimits);
    }

    if (!m_fitPlots) {
        // We want to round the automated limits to contextually whole numbers like 93 -> 100
        // TODO: Should the following block go in a separate function? roundBorders(min, max, roundings[])
        qreal maxMag = qMax(qAbs(minY), qAbs(maxY));
        // TODO: Using the <math.h> log10 may need a macro for handling different qreals on ARM platforms...
        qreal scale = qPow(10, qFloor(log10(maxMag)));
        qreal scaledMin = minY/scale, scaledMax = maxY/scale;

        minY = -1, maxY = -1;
        qreal deltaMin = 100, deltaMax = 100; // We know that 1 <= maxMag < 10
        foreach (qreal lim, m_yLimitRounding) {
            qreal dMin = scaledMin - sign(scaledMin)*lim; // Rounding down
            qreal dMax = sign(scaledMax)*lim - scaledMax; // Rounding up

            if (dMin >= 0 && dMin < deltaMin) {
                deltaMin = dMin;
                minY = sign(scaledMin)*lim;
            }
            if (dMax >= 0 && dMax < deltaMax) {
                deltaMax = dMax;
                maxY = sign(scaledMax)*lim;
            }
        }

        minY *= scale;
        maxY *= scale;
    }

    qDebug() << "Rounded limits" << minX << maxX << minY << maxY;
    qDebug() << "Current limits" << this->minX() << this->maxX() << this->minY() << this->maxY();
    qDebug() << "Floating" << m_minXFloat << m_maxXFloat << m_minYFloat << m_maxYFloat;

    // Update signals for undefined mins and maxs
    m_settingLimits = true;
    if (m_minXFloat)
        setMinX(minX, false);
    if (m_maxXFloat)
        setMaxX(maxX, false);
    if (m_minYFloat)
        setMinY(minY, false);
    if (m_maxYFloat)
        setMaxY(maxY, false);
    m_settingLimits = false;
    updateLimits();
}

QRectF Axis2DBase::maintainAspectRatio(QRectF guess, QPointF center)
{
    QRectF lim = guess;
    if (m_aspectRatio <= 0) return lim;
    if (lim.width() == 0 || lim.height() == 0) return lim;
    // Allow for machine precision errors...
    qreal eps = lim.width() * qPow(10, -10);
    QSizeF size = plotRect().size();
    if (size.width() == 0 || size.height() == 0) return lim;

    Util::AnchorSide anchor;
//    QPointF center = QPointF(0.5, 0.5);
    if (floatingLimits()) {
        // Refer back to the actual data limits
        if (m_minXFloat) lim.setLeft(m_dataLimits.left());
        if (m_maxXFloat) lim.setRight(m_dataLimits.right());
        if (m_minYFloat) lim.setTop(m_dataLimits.top());
        if (m_maxYFloat) lim.setBottom(m_dataLimits.bottom());

        if (!m_minXFloat && !m_maxXFloat) {
            // Here we give preference to anchoring the width
            anchor = Util::AnchorWidth;
            if (!m_minYFloat) center.setY(0);
            else if (!m_maxYFloat) center.setY(1);
//            else center.setY(0.5);

        } else if (!m_minYFloat && !m_maxYFloat) {
            anchor = Util::AnchorHeight;
            if (!m_minXFloat) center.setX(0);
            else if (!m_maxXFloat) center.setX(1);
//            else center.setX(0.5);

        } else {
            anchor = Util::AnchorFit;
        }

    } else {
        qreal dx = qAbs(lim.width() - limits().width());
        qreal dy = qAbs(lim.height() - limits().height());
        if (dx < eps && dy < eps) {
            anchor = Util::AnchorFit; // Or return
        } else if (dx < eps) {
            // Height Changed, make sure height is good
            anchor = Util::AnchorHeight;
        } else if (dy < eps) {
            // Width changed, make sure width good
            anchor = Util::AnchorWidth;
        } else if (dx > dy) {
            anchor = Util::AnchorWidth;
        } else {
            anchor = Util::AnchorHeight;
        }
    }
    // TODO: Remove debug
//    qDebug() << "Maintain aspect" << *lim << size << m_aspectRatio << anchor << center;
    Util::resizeRelativeRatio(&lim, size, m_aspectRatio, anchor, center);
//    qDebug() << "result" << *lim;
    return lim;
}

bool Axis2DBase::floatingLimits()
{
    return m_minXFloat || m_maxXFloat || m_minYFloat || m_maxYFloat;
}

qreal Axis2DBase::sign(qreal a)
{
    return (a > 0) - (a < 0);
}

/*!
 * \property Axis2DBase::limits
 * Sets all the data limits of the Axis at once.
 * \sa minX, maxX, minY, maxY
 */
QRectF Axis2DBase::limits() const
{
    return m_limits;
}

void Axis2DBase::setLimits(QRectF arg, bool fix, bool shareUpdate)
{
    if (m_limits == arg || m_settingLimits) return;
    m_settingLimits = true;
    if (fix)
        m_minXFloat = m_maxXFloat = m_minYFloat = m_maxYFloat = false;

    m_limits = maintainAspectRatio(arg);

    setMinX(m_limits.left(), fix, false);
    setMaxX(m_limits.right(), fix, false);
    m_xAxis->setLimits(m_minX, m_maxX);

    setMinY(m_limits.top(), fix, false);
    setMaxY(m_limits.bottom(), fix, false);
    m_yAxis->setLimits(m_minY, m_maxY);

    // If this wasn't called by figure->updateShareX, then call an update
    if (!shareUpdate && !m_shareX.isEmpty())
        figure()->updateShareX(this);
    if (!shareUpdate && !m_shareY.isEmpty())
        figure()->updateShareY(this);

    emit limitsChanged(arg);
    m_settingLimits = false;
}

void Axis2DBase::resetLimits()
{
    m_settingLimits = true;
    setMinX(-Inf);
    setMinY(-Inf);
    setMaxX(Inf);
    setMaxY(Inf);
    m_settingLimits = false;
    updateLimits();
}

QRectF Axis2DBase::dataLimits() const
{
    return m_dataLimits;
}

/*! \internal
 * \property Axis2DBase::yLimitRounding
 * A list of numbers between 1 and 10. It
 * defines the rounding used when auto-scaling the axes to fit the data. These
 * values are adapted for differing magnitudes of data. For example, the value
 * 1.5 will account for magnitudes like 0.15, 1.5, 15, 150, 1500, etc.
 * \return
 */
QList<qreal> Axis2DBase::yLimitRounding() const
{
    return m_yLimitRounding;
}

void Axis2DBase::setYLimitRounding(QList<qreal> arg)
{
    if (m_yLimitRounding == arg) return;
    m_yLimitRounding = arg;
    emit yLimitRoundingChanged(arg);
}

/*!
 * \property Axis2DBase::xAxis
 * The x-axis AxisSpec object for this Axis. This provides access to the axis
 * label, the major and minor ticks, etc.
 * \sa AxisSpec
 */
AxisSpec *Axis2DBase::xAxis() const
{
    return m_xAxis;
}

/*!
 * \property Axis2DBase::yAxis
 * The y-axis AxisSpec object for this Axis. This provides access to the axis
 * label, the major and minor ticks, etc.
 * \sa AxisSpec
 */
AxisSpec *Axis2DBase::yAxis() const
{
    return m_yAxis;
}

AxisMargins *Axis2DBase::margin() const
{
    return m_margin;
}

/*!
 * \property Axis2DBase::aspectRatio
 * If \a aspectRatio is larger than 0, the axis will enforce this ratio at all
 * times. Note: aspectRatio = x/y
 */
qreal Axis2DBase::aspectRatio() const
{
    return m_aspectRatio;
}

void Axis2DBase::setAspectRatio(qreal arg)
{
    if (m_aspectRatio == arg) return;
    m_aspectRatio = arg;
    emit aspectRatioChanged(arg);
}

/*!
 * \property Axis2DBase::fitPlots
 * If true, the automatic axis limits will fit the plots in the axis as
 * tightly as possible. If false, the axis will round the y-axis up to whole
 * numbers (dependent on the scale of the axis).
 * E.g. 894 -> 900, 8894 -> 9000
 */
bool Axis2DBase::fitPlots() const
{
    return m_fitPlots;
}

void Axis2DBase::setFitPlots(bool arg)
{
    if (m_fitPlots == arg) return;
    m_fitPlots = arg;
    emit fitPlotsChanged(arg);
}

QQuickPaintedItem *Axis2DBase::canvas() const
{
    return m_canvas;
}

void Axis2DBase::setCanvas(QQuickPaintedItem *arg)
{
    if (m_canvas == arg) return;
    m_canvas = arg;
    emit canvasChanged(arg);
}

QString Axis2DBase::shareX() const
{
    return m_shareX;
}

void Axis2DBase::setShareX(QString arg)
{
    if (m_shareX == arg)
        return;
    m_shareX = arg;

    emit shareXChanged(arg);
}

QString Axis2DBase::shareY() const
{
    return m_shareY;
}

void Axis2DBase::setShareY(QString arg)
{
    if (m_shareY == arg)
        return;

    m_shareY = arg;
    emit shareYChanged(arg);
}

/*!
 * \property Axis2DBase::grid
 * Read-only property provides access to specifying the Axis grid.
 * E.g.
 * \code{.qml}
 * grid.axis: "XY"
 * \endcode
 * \sa AxisGrid
 */
AxisGrid *Axis2DBase::grid() const
{
    return m_grid;
}

// ------------------------------------------------------------------
//          Grid
// ------------------------------------------------------------------

AxisGrid::AxisGrid(QObject *parent)
    : QObject(parent)
    , NutmegObject(this)
    , m_axis(AxisGrid::None)
    , m_majorLine(new LineSpec(":", parent))
    , m_minorLine(new LineSpec("", parent))
{
    m_majorLine->setColor(Qt::lightGray);

    QMap<QString, QString> props;
    props.insert("axes", "axes");
    props.insert("majorLine", "majorLine");
    props.insert("minorLine", "minorLine");
    registerProperties(props);
}

QString AxisGrid::mapProperty(const QString &prop)
{
    return NutmegObject::mapProperty(prop);
}

/*!
 * \property AxisGrid::axes
 * Set which axes for which the grid is enabled. Can take the following values:
 * "None", "X", "Y", "XY"
 * By default, this value is "None"
 */
AxisGrid::GridAxes AxisGrid::axes() const
{
    return m_axis;
}

void AxisGrid::setAxes(AxisGrid::GridAxes arg)
{
    if (m_axis == arg) return;
    m_axis = arg;
    emit axesChanged(arg);
}

/*!
 * \property Grid::majorLine
 * The default spec for grid lines for all axes. If an individual axis specifies its own
 * spec for the grid lines, this is ignored.
 * This is a \l LineSpec grouped property.
 * \sa LineSpec
 */
LineSpec *AxisGrid::majorLine() const
{
    return m_majorLine;
}

/*!
 * \property Grid::minorLine
 * The default spec for minor grid lines for all axes. If an individual axis specifies its own
 * spec for the grid lines, this is ignored.
 * This is a \l LineSpec grouped property.
 * \sa LineSpec
 */
LineSpec *AxisGrid::minorLine() const
{
    return m_minorLine;
}


// ------------------------------------------------------------------
//          AxisMargins
// ------------------------------------------------------------------

AxisMargins::AxisMargins(QObject *parent)
    : QObject(parent)
    , m_left(0)
    , m_right(0)
    , m_top(0)
    , m_bottom(0)
{
}

qreal AxisMargins::left() const
{
    return m_left;
}

void AxisMargins::setLeft(qreal arg)
{
    if (m_left == arg) return;
    m_left = arg;
    emit leftChanged(arg);
}

qreal AxisMargins::right() const
{
    return m_right;
}

void AxisMargins::setRight(qreal arg)
{
    if (m_right == arg) return;
    m_right = arg;
    emit rightChanged(arg);
}

qreal AxisMargins::top() const
{
    return m_top;
}

void AxisMargins::setTop(qreal arg)
{
    if (m_top == arg) return;
    m_top = arg;
    emit topChanged(arg);
}

qreal AxisMargins::bottom() const
{
    return m_bottom;
}

void AxisMargins::setBottom(qreal arg)
{
    if (m_bottom == arg) return;
    m_bottom = arg;
    emit bottomChanged(arg);
}
