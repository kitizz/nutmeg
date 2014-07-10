#include "axisbase.h"
#include "util.h"
#include <math.h>
#include <QtCore/qmath.h>

#define Font QFont

AxisBase::AxisBase(QQuickItem *parent)
    : QQuickPaintedItem(parent)
    , m_figure(0)
    , m_handle(QString())
    , m_plots(QMultiMap<QString,PlotBase*>())
    , m_minX(-Inf)
    , m_maxX(Inf)
    , m_minY(-Inf)
    , m_maxY(Inf)
    , m_minXFloat(true)
    , m_maxXFloat(true)
    , m_minYFloat(true)
    , m_maxYFloat(true)
    , m_plotsVar(QVariantMap())
    , m_destroying(false)
    , m_grid(new AxisGrid(parent))
    , m_xAxis(new AxisSpec(parent))
    , m_yAxis(new AxisSpec(parent))
    , m_yLimitRounding(QList<qreal>())
    , m_margin(new AxisMargins())
    , m_settingLimits(false)
    , m_aspectRatio(0)
    , m_fitPlots(false)
    , m_title("")
    , m_titleFont(QFont())
    , m_titleColor(QColor("black"))
{
    // Set initial axis label roundings
    m_yLimitRounding << 0 << 1 << 1.5 << 2 << 2.5 << 3 << 4 << 5 << 10;
    // Initialise the limits to inf. This defaults the limits to the data.
    m_limits = QRectF();
    m_limits.setTop(-Inf);
    m_limits.setLeft(-Inf);
    m_limits.setBottom(Inf);
    m_limits.setRight(Inf);

    // Initialise fonts
    m_titleFont.setBold(true);

    // Register properties available through the API
    QMap<QString,QString> props;
    props.insert("minX", "minX");
    props.insert("maxX", "maxX");
    props.insert("minY", "minY");
    props.insert("maxY", "maxY");
    props.insert("xAxis", "xAxis");
    props.insert("yAxis", "yAxis");
    props.insert("grid", "grid");
    props.insert("title", "title");
    registerProperties(props);
    connect(this, &QQuickItem::parentChanged, this, &AxisBase::updateFigure);

    // Connections for X axis lengths
    connect(this, &QQuickItem::widthChanged, this, &AxisBase::updateXAxis);
    connect(m_margin, &AxisMargins::leftChanged, this, &AxisBase::updateXAxis);
    connect(m_margin, &AxisMargins::rightChanged, this, &AxisBase::updateXAxis);
    connect(this, &AxisBase::minXChanged, [=](qreal val){ m_xAxis->setMin(val); });
    connect(this, &AxisBase::maxXChanged, [=](qreal val){ m_xAxis->setMax(val); });
    m_xAxis->setObjectName("XAxis");
    m_xAxis->setMin(minX());
    m_xAxis->setMax(maxX());
    m_xAxis->setPixelSize(width());

    // Connections for Y axis lengths
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
    connect(this, &QQuickItem::widthChanged, this, &AxisBase::updateLimits);
    connect(this, &QQuickItem::heightChanged, this, &AxisBase::updateLimits);

    // Connections for axis updates
    connect(m_xAxis, &AxisSpec::ticksChanged, this, &AxisBase::xAxisChanged);
    connect(m_yAxis, &AxisSpec::ticksChanged, this, &AxisBase::yAxisChanged);
}

AxisBase::~AxisBase()
{
    m_destroying = true;
}

qreal AxisBase::log_10(qreal v)
{
    return log10(v);
}

/*! \internal
 * \fn AxisBase::formatReal
 * Given \a num, and \a precision, return a string representation of the number
 * where exponentials are used if the magnitude of the number is > 10^precision
 * or < 10^-precision.
 * The \a forceNoExp parameter forces the format to be in plain decimal.
 */
QString AxisBase::formatReal(qreal num, int precision, int minMag, int maxMag)
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

qreal AxisBase::offsetFromStd(qreal val, qreal std)
{
    int precStd = (int)qFloor(log10( qAbs(std) ));
    return qRound64(val*qPow(10, -precStd))*qPow(10, precStd);
}

void AxisBase::paint(QPainter *painter)
{
    Q_UNUSED(painter)
}

FigureBase *AxisBase::figure() const
{
    return m_figure;
}

qreal AxisBase::minX() const
{
    return m_minX;
}

void AxisBase::setMinX(qreal arg, bool fix)
{
    if (arg == -Inf) arg = m_dataLimits.left();
    if (fix && m_minXFloat) m_minXFloat = false;
    if (m_minX == arg) return;

    m_minX = arg;
    emit minXChanged(arg);

}

qreal AxisBase::maxX() const
{
    return m_maxX;
}

void AxisBase::setMaxX(qreal arg, bool fix)
{
    if (arg == Inf) arg = m_dataLimits.right();
    if (fix && m_maxXFloat) m_maxXFloat = false;
    if (m_maxX == arg) return;

    m_maxX = arg;
    emit maxXChanged(arg);
}

qreal AxisBase::minY() const
{
    return m_minY;
}

void AxisBase::setMinY(qreal arg, bool fix)
{
    if (arg == -Inf) arg = m_dataLimits.top();
    if (fix && m_minYFloat) m_minYFloat = false;
    if (m_minY == arg) return;

    m_minY = arg;
    emit minYChanged(arg);
}

qreal AxisBase::maxY() const
{
    return m_maxY;
}

void AxisBase::setMaxY(qreal arg, bool fix)
{
    if (arg == Inf) arg = m_dataLimits.bottom();
    if (fix && m_maxYFloat) m_maxYFloat = false;
    if (m_maxY == arg) return;

    m_maxY = arg;
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

    // Work up the tree until the next Figure item is found.
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

    maintainAspectRatio(&newLimits);

    setLimits(newLimits, false);
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

void AxisBase::maintainAspectRatio(QRectF *lim)
{
    if (m_aspectRatio <= 0) return;
    // Allow for machine precision errors...
    qreal eps = lim->width() * qPow(10, -10);
    QSizeF size = QSizeF(width(), height());

    Util::AnchorSide anchor;
    QPointF center = QPointF(0.5, 0.5);
    if (floatingLimits()) {
        // Refer back to the actual data limits
        if (m_minXFloat) lim->setLeft(m_dataLimits.left());
        if (m_maxXFloat) lim->setRight(m_dataLimits.right());
        if (m_minYFloat) lim->setTop(m_dataLimits.top());
        if (m_maxYFloat) lim->setBottom(m_dataLimits.bottom());

        if (!m_minXFloat && !m_maxXFloat) {
            anchor = Util::AnchorWidth;
            if (!m_minYFloat) center.setY(0);
            else if (!m_maxYFloat) center.setY(1);
            else center.setY(0.5);

        } else if (!m_minYFloat && !m_maxYFloat) {
            anchor = Util::AnchorHeight;
            if (!m_minXFloat) center.setX(0);
            else if (!m_maxXFloat) center.setX(1);
            else center.setX(0.5);

        } else {
            anchor = Util::AnchorFit;
        }

    } else {
        qreal dx = qAbs(lim->width() - limits().width());
        qreal dy = qAbs(lim->height() - limits().height());
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
    qDebug() << "Maintain aspect" << *lim << size << m_aspectRatio << anchor << center;
    Util::resizeRelativeRatio(lim, size, m_aspectRatio, anchor, center);
    qDebug() << "result" << *lim;
}

bool AxisBase::floatingLimits()
{
    return m_minXFloat || m_maxXFloat || m_minYFloat || m_maxYFloat;
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

QString AxisBase::map(const QString &prop)
{
    return NutmegObject::map(prop);
}

QRectF AxisBase::limits() const
{
    return m_limits;
}

void AxisBase::setLimits(QRectF arg, bool fix)
{
    if (m_limits == arg || m_settingLimits) return;
    m_settingLimits = true;
    if (fix)
        m_minXFloat = m_maxXFloat = m_minYFloat = m_maxYFloat = false;

    maintainAspectRatio(&arg);
    m_limits = arg;

    setMinX(m_limits.left(), fix);
    setMaxX(m_limits.right(), fix);
    setMinY(m_limits.top(), fix);
    setMaxY(m_limits.bottom(), fix);

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

/*!
 * \property AxisBase::aspectRatio
 * If \a aspectRatio is larger than 0, the axis will enforce this ratio at all
 * times. Note: aspectRatio = x/y
 */
qreal AxisBase::aspectRatio() const
{
    return m_aspectRatio;
}

void AxisBase::setAspectRatio(qreal arg)
{
    if (m_aspectRatio == arg) return;
    m_aspectRatio = arg;
    emit aspectRatioChanged(arg);
}

/*!
 * \property AxisBase::fitPlots
 * If true, the automatic axis limits will fit the plots in the axis as
 * tightly as possible. If false, the axis will round the y-axis up to whole
 * numbers (dependent on the scale of the axis).
 * E.g. 894 -> 900, 8894 -> 9000
 */
bool AxisBase::fitPlots() const
{
    return m_fitPlots;
}

void AxisBase::setFitPlots(bool arg)
{
    if (m_fitPlots == arg) return;
    m_fitPlots = arg;
    emit fitPlotsChanged(arg);
}

/*!
 * \property AxisBase::title
 * Set the title of this axis.
 */
QString AxisBase::title() const
{
    return m_title;
}

void AxisBase::setTitle(QString arg)
{
    if (m_title == arg) return;
    m_title = arg;
    emit titleChanged(arg);
}

/*!
 * \property AxisBase::titleFont
 * Define the Font to be used for the title.
 * Can be used as a group property:
 * \code{.qml}
 * titleFont { bold: true, family: "Arial", pointSize: 16 }
 * \endcode
 * See QML's documentation for details on its basic Font type:
 * http://qt-project.org/doc/qt-5/qml-font.html
 */
Font AxisBase::titleFont() const
{
    return m_titleFont;
}

void AxisBase::setTitleFont(Font arg)
{
    if (m_titleFont == arg) return;
    m_titleFont = arg;
    emit titleFontChanged(arg);
}

/*!
 * \property AxisBase::titleColor
 * The color of the title text.
 */
QColor AxisBase::titleColor() const
{
    return m_titleColor;
}

void AxisBase::setTitleColor(QColor arg)
{
    if (m_titleColor == arg) return;
    m_titleColor = arg;
    emit titleColorChanged(arg);
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

    QMap<QString, QString> props;
    props.insert("axes", "axes");
    props.insert("majorLine", "majorLine");
    props.insert("minorLine", "minorLine");
    registerProperties(props);
}

QString AxisGrid::map(QString prop)
{
    return NutmegObject::map(prop);
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
    , m_majorLine(0)
    , m_minorLine(new LineSpec("", parent))
    , m_pixelSize(1)
    , m_min(0)
    , m_max(1)
    , m_inverted(false)
    , m_tickDir(In)
    , m_majorTickSize(5)
    , m_minorTickSize(2)
    , m_tickTextColor(QColor("black"))
    , m_tickFont(QFont())
{
    LineSpec *line = new LineSpec(parent);
    line->setColor("black");
    line->setWidth(2);
    m_majorLine = line;

    setMajorTicks(QVariant::fromValue(new AutoLocator(50)));
    m_ownMajorTicks = true;

    connect(this, &AxisSpec::majorTicksChanged, [=]() { emit ticksChanged(this); });
    connect(this, &AxisSpec::minorTicksChanged, [=]() { emit ticksChanged(this); });
    connect(this, &AxisSpec::invertedChanged, [=]() { emit ticksChanged(this); });

    QMap<QString, QString> props;
    props.insert("majorTicks", "majorTicks");
    props.insert("minorTicks", "minorTicks");
    props.insert("majorTickSize", "majorTickSize");
    props.insert("minorTickSize", "minorTickSize");
    props.insert("majorLine", "majorLine");
    props.insert("minorLine", "minorLine");
    props.insert("tickDir", "tickDir");
    props.insert("inverted", "inverted");
    props.insert("label", "label");
    registerProperties(props);
}

QString AxisSpec::map(QString prop)
{
    return NutmegObject::map(prop);
}


/*!
 * \property AxisSpec::majorTicks
 * \a majorTicks can be set any locator available.
 * By default, the majorTicks locator is an \l AutoLocator with density
 * set to 50 (pixels/tick).
 *
 * This property will also automatically create locators based on primitive
 * inputs. For example:
 * \code{.qml}
 * ...
 * majorTicks: 5
 * ...
 * \endcode
 * will create a \l SpacedLocator at intervals of 5,
 *
 * \code{.qml}
 * ...
 * majorTicks: [0, 5, 10, 20]
 * ...
 * \endcode
 * will create a \l HardLocator at the locations provided in the list.
 *
 * \code{.qml}
 * ...
 * majorTicks: null
 * ...
 * \endcode
 * will set the Locator back to the default AutoLocator.
 *
 * Reading this property will return the list of locations at that time (not
 * the Locator object, itself!)
 *
 * \sa AutoLocator, HardLocator, SpacedLocator
 */
QVariantList AxisSpec::majorTicks() const
{
    QVariantList lst;
    if (m_majorTicks) {
        foreach (qreal val, m_majorTicks->locations())
            lst << val;
    }

    return lst;
}

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

    } else if (arg.isNull()){
        qDebug() << "Set major ticks to default";
        m_majorTicks = new AutoLocator(50);
        m_ownMajorTicks = true;
    } else {
        qWarning() << "Warning: Axis.majorTick Locator provided is invalid.";
        return;
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

bool AxisSpec::inverted() const
{
    return m_inverted;
}

void AxisSpec::setInverted(bool arg)
{
    if (m_inverted == arg) return;
    m_inverted = arg;
    emit invertedChanged(arg);
}

/*!
 * \property AxisSpec::tickDir
 * Enum with the values: "Off", "In", "Out", "InOut"
 * This sets the directions of the ticks along this axis.
 */
AxisSpec::TickDirection AxisSpec::tickDir() const
{
    return m_tickDir;
}

void AxisSpec::setTickDir(AxisSpec::TickDirection arg)
{
    if (m_tickDir == arg) return;
    m_tickDir = arg;
    emit tickDirChanged(arg);
}

/*!
 * \property AxisSpec::majorTickSize
 * The length of the major ticks when they are drawn.
 * Default: 5
 */
qreal AxisSpec::majorTickSize() const
{
    return m_majorTickSize;
}

void AxisSpec::setMajorTickSize(qreal arg)
{
    if (m_majorTickSize == arg) return;
    m_majorTickSize = arg;
    emit majorTickSizeChanged(arg);
}

/*!
 * \property AxisSpec::minorTickSize
 * The length of the minor ticks when they are drawn.
 * Default: 2
 */
qreal AxisSpec::minorTickSize() const
{
    return m_minorTickSize;
}

void AxisSpec::setMinorTickSize(qreal arg)
{
    if (m_minorTickSize == arg) return;
    m_minorTickSize = arg;
    emit minorTickSizeChanged(arg);
}

/*!
 * \property AxisSpec::tickTextColor
 * The color of the tick numbers.
 */
QColor AxisSpec::tickTextColor() const
{
    return m_tickTextColor;
}

void AxisSpec::setTickTextColor(QColor arg)
{
    if (m_tickTextColor == arg) return;
    m_tickTextColor = arg;
    emit tickTextColorChanged(arg);
}

/*!
 * \property AxisSpec::tickFont
 * The font used for the tick numbers
 */
QFont AxisSpec::tickFont() const
{
    return m_tickFont;
}

void AxisSpec::setTickFont(QFont arg)
{
    if (m_tickFont == arg) return;
    m_tickFont = arg;
    emit tickFontChanged(arg);
}

/*!
 * \property AxisSpec::label
 * The label for this axis.
 */
QString AxisSpec::label() const
{
    return m_label;
}

void AxisSpec::setLabel(QString arg)
{
    if (m_label == arg) return;
    m_label = arg;
    emit labelChanged(arg);
}

/*!
 * \property AxisSpec::labelColor
 * The color of label for this axis.
 */
QColor AxisSpec::labelColor() const
{
    return m_labelColor;
}

void AxisSpec::setLabelColor(QColor arg)
{
    if (m_labelColor == arg) return;
    m_labelColor = arg;
    emit labelColorChanged(arg);
}

/*!
 * \property AxisSpec::labelFont
 * The font of the label for this axis.
 */
QFont AxisSpec::labelFont() const
{
    return m_labelFont;
}

void AxisSpec::setLabelFont(QFont arg)
{
    if (m_labelFont == arg) return;
    m_labelFont = arg;
    emit labelFontChanged(arg);
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
