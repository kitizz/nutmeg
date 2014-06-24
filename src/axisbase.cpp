#include "axisbase.h"
#include <math.h>
#include <QtCore/qmath.h>

AxisBase::AxisBase(QQuickItem *parent)
    : QQuickPaintedItem(parent)
    , m_figure(0)
    , m_handle(QString())
    , m_plots(QMultiMap<QString,PlotBase*>())
    , m_minX(-Inf)
    , m_maxX(Inf)
    , m_minY(-Inf)
    , m_maxY(Inf)
    , m_plotsVar(QVariantMap())
    , m_destroying(false)
    , m_grid(new AxisGrid(parent))
    , m_xAxis(new AxisSpec(parent))
    , m_yAxis(new AxisSpec(parent))
    , m_yLimitRounding(QList<qreal>())
    , m_margin(new AxisMargins())
    , m_settingLimits(false)
{
    m_yLimitRounding << 0 << 1 << 1.5 << 2 << 2.5 << 3 << 4 << 5 << 10;
    m_limits = QRectF();
    m_limits.setTop(-Inf);
    m_limits.setLeft(-Inf);
    m_limits.setBottom(Inf);
    m_limits.setRight(Inf);

    QMap<QString,QString> props;
    props.insert("minX", "minX");
    props.insert("maxX", "maxX");
    props.insert("minY", "minY");
    props.insert("maxY", "maxY");
    registerProperties(props);
    connect(this, &QQuickItem::parentChanged, this, &AxisBase::updateFigure);

    // Connections for axis lengths
    connect(this, &QQuickItem::widthChanged, this, &AxisBase::updateXAxis);
    connect(m_margin, &AxisMargins::leftChanged, this, &AxisBase::updateXAxis);
    connect(m_margin, &AxisMargins::rightChanged, this, &AxisBase::updateXAxis);
//    connect(this, &QQuickItem::widthChanged, this, &AxisBase::updateXAxis);
    connect(this, &AxisBase::minXChanged, [=](qreal val){ m_xAxis->setMin(val); });
    connect(this, &AxisBase::maxXChanged, [=](qreal val){ m_xAxis->setMax(val); });
    m_xAxis->setObjectName("XAxis");
    m_xAxis->setMin(minX());
    m_xAxis->setMax(maxX());
    m_xAxis->setPixelSize(width());

    connect(this, &QQuickItem::heightChanged, this, &AxisBase::updateYAxis);
    connect(m_margin, &AxisMargins::topChanged, this, &AxisBase::updateYAxis);
    connect(m_margin, &AxisMargins::bottomChanged, this, &AxisBase::updateYAxis);
    connect(this, &AxisBase::minYChanged, [=](qreal val){ m_yAxis->setMin(val); });
    connect(this, &AxisBase::maxYChanged, [=](qreal val){ m_yAxis->setMax(val); });
    m_yAxis->setObjectName("YAxis");
    m_yAxis->setMin(minY());
    m_yAxis->setMax(maxY());
    m_yAxis->setPixelSize(height());

    // Connections for limits
    connect(this, &AxisBase::minXChanged, this, &AxisBase::updateLimits);
    connect(this, &AxisBase::minYChanged, this, &AxisBase::updateLimits);
    connect(this, &AxisBase::maxXChanged, this, &AxisBase::updateLimits);
    connect(this, &AxisBase::maxYChanged, this, &AxisBase::updateLimits);
    connect(this, &AxisBase::dataLimitsChanged, this, &AxisBase::updateLimits);
}

AxisBase::~AxisBase()
{
    m_destroying = true;
}

qreal AxisBase::log_10(qreal v)
{
    return log10(v);
}

/*!
 * \fn AxisBase::formatReal
 * Given \a num, and \a precision, return a string representation of the number
 * where exponentials are used if the magnitude of the number is > 10^precision
 * or < 10^-precision.
 * The \a forceNoExp parameter forces the format to be in plain decimal.
 */
QString AxisBase::formatReal(qreal num, int precision, int maxMag)
{
    QString str;
    qreal abNum = qAbs(num);

    if (maxMag < 0)
        maxMag = precision;

    if (abNum < qPow(10, -15)) {
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

qreal AxisBase::offsetFromStd(qreal val, qreal std)
{
    int precStd = (int)qFloor(log10( qAbs(std) ));
//    int precMn = (int)qFloor(log10( qAbs(mean) ));
    return qRound(val*qPow(10, -precStd))*qPow(10, precStd);
}

void AxisBase::paint(QPainter *painter)
{

}

FigureBase *AxisBase::figure() const
{
    return m_figure;
}

qreal AxisBase::minX() const
{
    if (m_minX == -Inf)
        return m_dataLimits.left();
    return m_minX;
}

void AxisBase::setMinX(qreal arg)
{
//    arg = qMax(arg, m_dataLimits.left() - m_dataLimits.width()/2);
    if (m_minX == arg) return;
    m_minX = arg;
    if (arg == -Inf)
        emit minXChanged(m_dataLimits.left());
    else
        emit minXChanged(arg);

}

qreal AxisBase::maxX() const
{
    if (m_maxX == Inf)
        return m_dataLimits.right();
    return m_maxX;
}

void AxisBase::setMaxX(qreal arg)
{
//    arg = qMin(arg, m_dataLimits.right() + m_dataLimits.width()/2);
    if (m_maxX == arg) return;
    m_maxX = arg;
    if (arg == Inf)
        emit maxXChanged(m_dataLimits.right());
    else
        emit maxXChanged(arg);
}

qreal AxisBase::minY() const
{
    if (m_minY == -Inf)
        return m_dataLimits.top();
    return m_minY;
}

void AxisBase::setMinY(qreal arg)
{
    if (m_minY == arg) return;
    m_minY = arg;
    if (arg == -Inf)
        emit minYChanged(m_dataLimits.top());
    else
        emit minYChanged(arg);
}

qreal AxisBase::maxY() const
{
    if (m_maxY == Inf)
        return m_dataLimits.bottom();
    return m_maxY;
}

void AxisBase::setMaxY(qreal arg)
{
    if (m_maxY == arg) return;
    m_maxY = arg;
    if (arg == Inf)
        emit maxYChanged(m_dataLimits.bottom());
    else
        emit maxYChanged(arg);
}

void AxisBase::offset(qreal x, qreal y)
{
    // TODO: Implement AxisBase::offset
}

QString AxisBase::handle() const
{
    return m_handle;
}

void AxisBase::setHandle(QString arg)
{
    if (m_handle == arg) return;
    m_handle = arg;
    emit handleChanged(arg);
}

QVariantMap AxisBase::plots()
{
    return m_plotsVar;
}

void AxisBase::setFigure(FigureBase *arg)
{
    if (m_figure == arg) return;

    if (m_figure) {
        m_figure->deregisterAxis(this);
    }

    m_figure = arg;

    if (m_figure) {
        m_figure->registerAxis(this);
    }
//    else {
//        qWarning() << Q_FUNC_INFO << "Axis is not a descendant of any Figure. It may not behave as expected.";
//    }

    emit figureChanged(arg);
}

QVariantList AxisBase::getPlotList()
{
    return m_plotsList;
}

void AxisBase::updateFigure()
{
    // Disconnect from an external sender
    QObject *sender = QObject::sender();
    if (sender && this != sender)
        sender->disconnect(this);

    // Work up the tree until the next Axis item is found.
    QQuickItem *newParent = parentItem();
    if (!newParent) return; // Either being deleted or instantiated
    FigureBase *figure;
    while (true) {
        figure = qobject_cast<FigureBase*>(newParent);
        if (figure) break;
        if (!newParent->parentItem()) {
            connect(newParent, &QQuickItem::parentChanged, this, &AxisBase::updateFigure);
            break;
        }
        newParent = newParent->parentItem();
    }

    setFigure(figure);
}

void AxisBase::updateXAxis()
{
    qreal w = width() - m_margin->left() - m_margin->right();
    m_xAxis->setPixelSize(w);
}

void AxisBase::updateYAxis()
{
    qreal h = height() - m_margin->top() - m_margin->bottom();
    m_yAxis->setPixelSize(h);
}

void AxisBase::updatePlots()
{
    QVariantMap newMap; // Map of QVariantList
    QVariantList newList;
    foreach (const QString &key, m_plots.keys()) {
        QVariantList list;
        foreach (QObject *plt, m_plots.values(key)) {
            list.prepend(QVariant::fromValue(plt));
            newList.prepend(QVariant::fromValue(plt));
        }
        newMap.insert(key, list);
    }
    m_plotsVar = newMap;
    m_plotsList = newList;
    emit plotsChanged(m_plotsVar);
}

void AxisBase::updateLimits()
{
    if (m_settingLimits) return;
    QRectF newLimits;
    newLimits.setLeft(minX());
    newLimits.setTop(minY());
    newLimits.setRight(maxX());
    newLimits.setBottom(maxY());

    if (newLimits == m_limits) return;
    m_limits = newLimits;
    emit limitsChanged(m_limits);
}

void AxisBase::updateDataLimits()
{
    qreal minX = Inf, maxX = -Inf, minY = Inf, maxY = -Inf;

    bool validLimits = false;
    foreach (PlotBase* plot, m_plots.values()) {
        QRectF rect = plot->dataLimits();
        if (rect.isEmpty()) continue;
        validLimits = true;

        if (rect.left() < minX) minX = rect.left();
        if (rect.right() > maxX) maxX = rect.right();
        if (rect.top() < minY) minY = rect.top();
        if (rect.bottom() > maxY) maxY = rect.bottom();
    }

    if (!validLimits) return;

    // We want to round the automated limits to contextually whole numbers like 93 -> 100
    // TODO: Should the following block go in a separate function? roundBorders(min, max, roundings[])
    qreal maxMag = qMax(qAbs(minY), qAbs(maxY));
    // TODO: Using the <math.h> log10 may need a macro for handling different qreals on ARM platforms...
    qreal scale = qPow(10, qFloor(log10(maxMag)));
    qreal scaledMin = minY/scale, scaledMax = maxY/scale;

    qreal newMinY = -1, newMaxY = -1;
    qreal deltaMin = 100, deltaMax = 100; // We know that 1 <= maxMag < 10
    foreach (qreal lim, m_yLimitRounding) {
        qreal dMin = scaledMin - sign(scaledMin)*lim; // Rounding down
        qreal dMax = sign(scaledMax)*lim - scaledMax; // Rounding up

        if (dMin >= 0 && dMin < deltaMin) {
            deltaMin = dMin;
            newMinY = sign(scaledMin)*lim;
        }
        if (dMax >= 0 && dMax < deltaMax) {
            deltaMax = dMax;
            newMaxY = sign(scaledMax)*lim;
        }
    }

    newMinY *= scale;
    newMaxY *= scale;

    QRectF rect;
    rect.setLeft(minX);
    rect.setRight(maxX);
    rect.setTop(newMinY);
    rect.setBottom(newMaxY);
    if (m_dataLimits != rect) {
        m_dataLimits = rect;
        emit dataLimitsChanged(m_dataLimits);
    }

    // Update signals for undefined mins and maxs
    if (m_minX == -Inf)
        emit minXChanged(minX);
    if (m_maxX == Inf)
        emit maxXChanged(maxX);
    if (m_minY == -Inf)
        emit minYChanged(newMinY);
    if (m_maxY == Inf)
        emit maxYChanged(newMaxY);
}

qreal AxisBase::sign(qreal a)
{
    return (a > 0) - (a < 0);
}

QList<PlotBase*> AxisBase::getPlotsByHandle(QString handle)
{
    // TODO: This is O(n) complexity at the moment...
    QList<PlotBase*> values;
    if (handle.isEmpty())
        values = m_plots.values();
    else
        values = m_plots.values(handle);

    QList<PlotBase*> result;
    foreach (PlotBase* plt, values)
        result.prepend(plt);

    return result;
}

QString AxisBase::map(QString prop)
{
    return NutmegObject::map(prop);
}

QRectF AxisBase::limits() const
{
    return m_limits;
}

void AxisBase::setLimits(QRectF arg)
{
    if (m_limits == arg || m_settingLimits) return;
    m_settingLimits = true;
    m_limits = arg;

    setMinX(m_limits.left());
    setMaxX(m_limits.right());
    setMinY(m_limits.top());
    setMaxY(m_limits.bottom());

    emit limitsChanged(arg);
    m_settingLimits = false;
}

void AxisBase::resetLimits()
{
    m_settingLimits = true;
    setMinX(-Inf);
    setMinY(-Inf);
    setMaxX(Inf);
    setMaxY(Inf);
    m_settingLimits = false;
    updateLimits();
}

QRectF AxisBase::dataLimits() const
{
    return m_dataLimits;
}

/*!
 * \brief AxisBase::yLimitRounding A list of numbers between 1 and 10. It
 * defines the rounding used when auto-scaling the axes to fit the data. These
 * values are adapted for differing magnitudes of data. For example, the value
 * 1.5 will account for magnitudes like 0.15, 1.5, 15, 150, 1500, etc.
 * \return
 */
QList<qreal> AxisBase::yLimitRounding() const
{
    return m_yLimitRounding;
}

void AxisBase::setYLimitRounding(QList<qreal> arg)
{
    if (m_yLimitRounding == arg) return;
    m_yLimitRounding = arg;
    emit yLimitRoundingChanged(arg);
}

AxisSpec *AxisBase::xAxis() const
{
    return m_xAxis;
}


AxisSpec *AxisBase::yAxis() const
{
    return m_yAxis;
}

AxisMargins *AxisBase::margin() const
{
    return m_margin;
}

AxisGrid *AxisBase::grid() const
{
    return m_grid;
}

void AxisBase::registerPlot(PlotBase *plot)
{
    QString key = plot->handle();

    if (m_plots.contains(key, plot))
        return; // Already in the list, move along now

    m_plots.insert(key, plot);
    connect(plot, &PlotBase::dataLimitsChanged, this, &AxisBase::updateDataLimits);
    updatePlots();
    updateDataLimits();
}

void AxisBase::deregisterPlot(PlotBase *plot)
{
    m_plots.remove(plot->handle(), plot);
    updatePlots();
}

// ------------------------------------------------------------------
//          Grid
// ------------------------------------------------------------------

AxisGrid::AxisGrid(QObject *parent)
    : QObject(parent)
    , m_axis(AxisGrid::None)
    , m_majorLine(new LineSpec(":", parent))
    , m_minorLine(new LineSpec("", parent))
{
    m_majorLine->setColor(Qt::lightGray);
}

AxisGrid::GridAxes AxisGrid::axis() const
{
    return m_axis;
}

void AxisGrid::setAxis(AxisGrid::GridAxes arg)
{
    if (m_axis == arg) return;
    m_axis = arg;
    emit axisChanged(arg);
}

/*!
 * \brief Grid::majorLine
 * The default spec for grid lines for all axes. If an individual axis specifies its own
 * spec for the grid lines, this is ignored.
 * \return A grouped property that specifies the style of line used for drawing major grid lines.
 */
LineSpec *AxisGrid::majorLine() const
{
    return m_majorLine;
}

/*!
 * \brief Grid::minorLine
 * The default spec for minor grid lines for all axes. If an individual axis specifies its own
 * spec, this is ignored.
 * \return A grouped property that specifies the style of line used for drawing minor grid lines.
 */
LineSpec *AxisGrid::minorLine() const
{
    return m_minorLine;
}


// ------------------------------------------------------------------
//          AxisSpec
// ------------------------------------------------------------------

AxisSpec::AxisSpec(QObject *parent)
    : QObject(parent)
    , m_majorTicks(0)
    , m_minorTicks(0)
    , m_majorTicksVar(QVariant())
    , m_minorTicksVar(QVariant())
    , m_ownMajorTicks(false)
    , m_ownMinorTicks(false)
    , m_majorLine(new LineSpec("", parent))
    , m_minorLine(new LineSpec("", parent))
    , m_pixelSize(1)
    , m_min(0)
    , m_max(1)
{
    setMajorTicks(QVariant::fromValue(new AutoLocator(50)));
    m_ownMajorTicks = true;
}

//AxisSpec::AxisSpec(QString sizeProperty, QObject *parent)
//    : AxisSpec(parent)
//{
//    // Thanks to: http://stackoverflow.com/questions/2799412/how-to-use-qmetamethod-with-qobjectconnect
//    // If the parent changes, bad things will happen...
//    const QMetaObject *parentMeta = parent->metaObject();
//    QMetaProperty prop = parentMeta->property(parentMeta->indexOfProperty(sizeProperty));
//    if (prop.hasNotifySignal()) {
//        QMetaMethod signal = prop.notifySignal();
//        connect(parent, signal, [=](){ setSize(parent->property(sizeProperty)) });
//    }
//}

QVariantList AxisSpec::majorTicks() const
{
    QVariantList lst;
    if (m_majorTicks) {
        foreach (qreal val, m_majorTicks->locations()) {
            lst << val;
        }
    }

    return lst;
}

/*!
 * \brief AxisSpec::setMajorTicks
 * \param arg
 */
void AxisSpec::setMajorTicks(QVariant arg)
{
    /* TODO: Allow the ticks to be set in the following ways:
     * majorTicks: null
     * majorTicks: 5  // A ticks at multiples of 5 (AutoLocator with multiples = [5]
     * majorTicks: [1, 3, 4, 5, 10]  // Ticks at specific, hard coded locations
     * majorTicks: AutoLocator { multiples: [1,5] }
     */
    if (m_majorTicksVar == arg) return;
    // Using the majorTicksVar to make it easy to check for changed values.
    m_majorTicksVar = arg;

    // Disconnect things
    if (m_majorTicks) {
        m_majorTicks->disconnect(this);
        if (m_ownMajorTicks)
            delete m_majorTicks;
    }

    qDebug() << "Setting major";

    if (arg.canConvert<QVariantList>()) {
        QList<qreal> locs;
        bool ok = true;
        foreach (QVariant var, arg.value<QVariantList>()) {
            qreal val = var.toReal(&ok);
            if (ok) locs << val;
        }
        qDebug() << "Setting major to hard locator";
        m_majorTicks = new HardLocator(locs);
        m_ownMajorTicks = true;

    } else if (arg.canConvert(QMetaType::QReal)) {
        m_majorTicks = new SpacedLocator(arg.toReal());
        m_ownMajorTicks = true;

    } else if (arg.canConvert<Locator*>()) {
        qDebug() << "Set major to existing locator";
        m_majorTicks = arg.value<Locator*>();
        m_ownMajorTicks = false;

    } else {
        qDebug() << "Set major to null";
        m_majorTicks = 0;
        m_ownMajorTicks = false;
    }

    if (m_majorTicks) {
        connect(m_majorTicks, &Locator::locationsChanged, this, &AxisSpec::majorTicksChanged);
        updateMajor();
    }

    emit majorTicksChanged(m_majorTicks->locations());
}

QVariantList AxisSpec::minorTicks() const
{
    QVariantList lst;
    if (m_minorTicks) {
        foreach (qreal val, m_minorTicks->locations()) {
            lst << val;
        }
    }

    return lst;
}

void AxisSpec::setMinorTicks(QVariant arg)
{
    if (m_minorTicksVar == arg) return;
    // Using the minorTicksVar to make it easy to check for changed values.
    m_minorTicksVar = arg;

    // Disconnect things
    if (m_minorTicks) {
        m_minorTicks->disconnect(this);
        if (m_ownMinorTicks)
            delete m_minorTicks;
    }

    if (arg.canConvert< QList<qreal> >()) {
        QList<qreal> locs;
        bool ok = true;
        foreach (QVariant var, arg.value<QVariantList>()) {
            qreal val = var.toReal(&ok);
            if (ok) locs << val;
        }
        m_minorTicks = new HardLocator(locs);
        m_ownMinorTicks = true;

    } else if (arg.canConvert(QMetaType::QReal)) {
        m_minorTicks = new SpacedLocator(arg.toReal());
        m_ownMinorTicks = true;

    } else if (arg.canConvert<Locator*>()) {
        m_minorTicks = arg.value<Locator*>();
        m_ownMinorTicks = false;

    } else {
        m_minorTicks = 0;
        m_ownMinorTicks = false;
    }

    if (m_minorTicks) {
        connect(m_minorTicks, &Locator::locationsChanged, this, &AxisSpec::minorTicksChanged);
        updateMinor();
    }

    emit minorTicksChanged(m_minorTicks->locations());
}

LineSpec *AxisSpec::majorLine() const
{
    return m_majorLine;
}

LineSpec *AxisSpec::minorLine() const
{
    return m_minorLine;
}

qreal AxisSpec::pixelSize() const
{
    return m_pixelSize;
}

void AxisSpec::setPixelSize(qreal arg)
{
    if (m_pixelSize == arg) return;
    m_pixelSize = arg;
    updateMajor();
    emit pixelSizeChanged(arg);
}

qreal AxisSpec::min() const
{
    return m_min;
}

void AxisSpec::setMin(qreal arg)
{
    if (m_min == arg) return;
    m_min = arg;
    updateMajor();
    emit minChanged(arg);
}

qreal AxisSpec::max() const
{
    return m_max;
}

void AxisSpec::setMax(qreal arg)
{
    if (m_max == arg) return;
    m_max = arg;
    updateMajor();
    emit maxChanged(arg);
}

void AxisSpec::updateMajor()
{
    if (!m_majorTicks) return;
    m_majorTicks->setPixelSize(m_pixelSize);
    m_majorTicks->setStart(m_min);
    m_majorTicks->setEnd(m_max);
}

void AxisSpec::updateMinor()
{
    if (!m_minorTicks) return;
    m_minorTicks->setPixelSize(m_pixelSize);
    m_minorTicks->setStart(m_min);
    m_minorTicks->setEnd(m_max);
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
