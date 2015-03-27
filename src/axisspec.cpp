#include "axisspec.h"

#include <QVariantMap>

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

    connect(this, &AxisSpec::majorTicksChanged, [=]() { emit AxisSpec::majorTickLabelsChanged(majorTickLabels()); });
    connect(this, &AxisSpec::minorTicksChanged, [=]() { emit AxisSpec::minorTickLabelsChanged(minorTickLabels()); });

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

QString AxisSpec::mapProperty(const QString &prop)
{
    return NutmegObject::mapProperty(prop);
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
 * \sa AutoLocator, HardLocator, SpacedLocator, AxisSpec::minorTicks
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
     * majorTicks: ["a":1, "b":2, "c":6] // Labels at specific, hard coded locations
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

    qDebug() << "AxisSpec::setMajorTicks:" << arg;

    if (arg.canConvert<QVariantMap>()) {
        QList<qreal> locs;
        QList<QString> labels;
        bool ok = true;
        QVariantMap map = arg.value<QVariantMap>();
        foreach (QString label, map.keys()) {
            labels.append(label);
            QVariant var = map[label];
            qreal loc = var.toReal(&ok);
            if (ok) locs << loc;
        }
        qDebug() << "AxisSpec::setMajorTicks: Setting major to label locator" << locs << labels;
        m_majorTicks = new LabelLocator(labels, locs);
        m_ownMajorTicks = true;

    } else if (arg.canConvert<QVariantList>()) {
        QList<qreal> locs;
        bool ok = true;
        foreach (QVariant var, arg.value<QVariantList>()) {
            qreal val = var.toReal(&ok);
            if (ok) locs << val;
        }
        qDebug() << "AxisSpec::setMajorTicks: Setting major to hard locator" << locs;
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
        m_majorTicks = 0;
        m_ownMajorTicks = false;
//        m_majorTicks = new AutoLocator(50);
//        m_ownMajorTicks = true;
    } else {
        qWarning() << "Warning: Axis.majorTick Locator provided is invalid.";
        return;
    }

    if (m_majorTicks) {
        connect(m_majorTicks, &Locator::locationsChanged, this, &AxisSpec::majorTicksChanged);
        updateMajor();
        emit majorTicksChanged(m_majorTicks->locations());
    } else {
        emit majorTicksChanged(QList<qreal>());
    }

}

/*!
 * \property AxisSpec::minorTicks
 * Define how the minor ticks are laid out. See \l AxisSpec::majorTicks
 * for more information.
 *
 * \sa AxisSpec::majorTicks
 */
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

QStringList AxisSpec::majorTickLabels() const
{
    if (m_majorTicks)
        return m_majorTicks->locationLabels();
    else
        return QStringList();
}

QStringList AxisSpec::minorTickLabels() const
{
    if (m_minorTicks)
        return m_minorTicks->locationLabels();
    else
        return QStringList();
}

/*!
 * \property AxisSpec::majorLine
 * Read-only LineSpec property provides access to the line style used for the
 * ticks.
 * E.g.
 * \code
 * majorLine.color: "red"
 * majorLine.width: 2
 * \endcode
 * \sa LineSpec, minorLine
 */
LineSpec *AxisSpec::majorLine() const
{
    return m_majorLine;
}

/*!
 * \property AxisSpec::majorLine
 * Read-only LineSpec property provides access to the line style used for the
 * ticks.
 * \sa LineSpec, majorLine
 */
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

void AxisSpec::setLimits(qreal min, qreal max)
{
    bool updateMin = m_min != min;
    bool updateMax = m_max != max;
    if (!updateMin && !updateMax) return;

    m_min = min;
    m_max = max;
    if (m_majorTicks)
        m_majorTicks->setLimits(m_min, m_max);

    if (updateMin)
        emit minChanged(min);
    if (updateMax)
        emit maxChanged(max);
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

void AxisSpec::updateMajor()
{
    if (!m_majorTicks) return;
    m_majorTicks->setPixelSize(m_pixelSize);
    m_majorTicks->setLimits(m_min, m_max);
}

void AxisSpec::updateMinor()
{
    if (!m_minorTicks) return;
    m_minorTicks->setPixelSize(m_pixelSize);
    m_minorTicks->setStart(m_min);
    m_minorTicks->setEnd(m_max);
}

