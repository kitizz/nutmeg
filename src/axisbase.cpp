#include "axisbase.h"

#include "defaults.h"

AxisBase::AxisBase(QQuickItem *parent)
    : QQuickItem(parent)
    , m_handle("axis")
    , m_figure(0)
    , m_plotRect(QRectF())
    , m_plotRectValid(false)
    , m_title("")
    , m_titleFont(Defaults::defaultFont())
    , m_titleColor(QColor(Qt::black))
    , m_plots(QMap<QString,PlotBase*>())
    , m_plotsVar(QVariantMap())
    , m_destroying(false)
{
    QMap<QString,QString> props;
    props.insert("handle", "handle");
    props.insert("title", "title");
//    props.insert("titleFont", "titleFont");
    registerProperties(props);

    // Initialize fonts
    m_titleFont.setBold(true);
}

AxisBase::~AxisBase()
{
    foreach (PlotBase* plt, m_plots.values()) {
        deregisterPlot(plt);
        delete plt;
    }
    m_destroying = true;
}

//void AxisBase::registerProperties(QMap<QString, QString> mapping)
//{
//    NutmegObject::registerProperties(mapping);
//}

//void AxisBase::registerProperties(QVariantMap mapping)
//{
//    QMap<QString, QString> map;
//    foreach (QString tag, mapping.keys()) {
//        QString prop = mapping.value(tag).toString();
//        if (!prop.isEmpty())
//            map.insert(tag, prop);
//    }
//    registerProperties(map);
//}

//QString AxisBase::mapProperty(const QString &prop)
//{
//    return NutmegObject::mapProperty(prop);
//}

QVariantMap AxisBase::plots() const
{
    return m_plotsVar;
}

PlotBase* AxisBase::getPlotByHandle(QString handle) const
{
    return m_plots[handle];
}

QList<QString> AxisBase::plotNames() const
{
    return m_plots.keys();
}

QVariantList AxisBase::getPlotList() const
{
    return m_plotsVar.values();
}

/*!
 * \property AxisBase::handle
 * The handle property provides Nutmeg a way of identifying this Axis.
 * By default, this is "axis".
 */
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

/*!
 * \property AxisBase::plotRect
 * A rectangle that defines the actual plotting area in this axis.
 */
QRectF AxisBase::plotRect() const
{
    if (m_plotRectValid)
        return m_plotRect;
    else
        return QRectF(0, 0, width(), height());
}

void AxisBase::setPlotRect(QRectF arg)
{
    QRectF oldRect = plotRect();
    m_plotRectValid = true;
    if (oldRect == arg) return;
    m_plotRect = arg;
    emit plotRectChanged(arg);
}

void AxisBase::resetPlotRect()
{
    QRectF oldRect = plotRect();
    m_plotRectValid = false;
    QRectF newRect = plotRect();
    if (newRect != oldRect)
        emit plotRectChanged(newRect);
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
QFont AxisBase::titleFont() const
{
    return m_titleFont;
}

void AxisBase::setTitleFont(QFont arg)
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

FigureBase *AxisBase::figure() const
{
    return m_figure;
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

void AxisBase::registerPlot(PlotBase *plot)
{
    QString key = plot->handle();

    if (m_plots.contains(key)) {
        // TODO: Send a warning to client about this...
        qWarning() << "Warning: Plot with this handle already exists in this Axis";
        return; // Already in the list, move along now
    }

    qDebug() << "Adding plot to axis:" << key;
    m_plots.insert(key, plot);
    emit addedPlot(plot);
    updatePlots();
}

void AxisBase::deregisterPlot(PlotBase *plot)
{
    m_plots.remove(plot->handle());
    emit removedPlot(plot);
    updatePlots();
}

void AxisBase::updatePlots()
{
    QVariantMap newMap; // Map of QVariantList
//    QVariantList newList;
    foreach (const QString &key, m_plots.keys()) {
        QVariant plt = QVariant::fromValue(m_plots[key]);
//        newList.prepend(plt);
        newMap.insert(key, plt);
    }
    m_plotsVar = newMap;
    emit plotsChanged(m_plotsVar);
}
