#include "axisbase.h"

AxisBase::AxisBase(QQuickItem *parent)
    : QQuickPaintedItem(parent)
    , m_figure(0)
    , m_handle(QString())
    , m_minX(-Inf)
    , m_maxX(Inf)
    , m_minY(-Inf)
    , m_maxY(Inf)
    , m_plots(QMultiMap<QString,PlotBase*>())
    , m_plotsVar(QVariantMap())
    , m_destroying(false)
{
    QMap<QString,QString> props;
    props.insert("minX", "minX");
    props.insert("maxX", "maxX");
    props.insert("minY", "minY");
    props.insert("maxY", "maxY");
    registerProperties(props);
    connect(this, &QQuickItem::parentChanged, this, &AxisBase::updateFigure);
}

AxisBase::~AxisBase()
{
    m_destroying = true;
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
    return m_minX;
}

void AxisBase::setMinX(qreal arg)
{
    if (m_minX == arg) return;
    m_minX = arg;
    emit minXChanged(arg);
}

qreal AxisBase::maxX() const
{
    return m_maxX;
}

void AxisBase::setMaxX(qreal arg)
{
    if (m_maxX == arg) return;
    m_maxX = arg;
    emit maxXChanged(arg);
}

qreal AxisBase::minY() const
{
    return m_minY;
}

void AxisBase::setMinY(qreal arg)
{
    if (m_minY == arg) return;
    m_minY = arg;
    emit minYChanged(arg);
}

qreal AxisBase::maxY() const
{
    return m_maxY;
}

void AxisBase::setMaxY(qreal arg)
{
    if (m_maxY == arg) return;
    m_maxY = arg;
    emit maxYChanged(arg);
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

void AxisBase::updatePlots()
{
    QVariantMap newMap; // Map of QVariantList
    foreach (const QString &key, m_plots.keys()) {
        QVariantList list;
        foreach (QObject *plt, m_plots.values(key))
            list.prepend(QVariant::fromValue(plt));
        newMap.insert(key, list);
    }
    m_plotsVar = newMap;
    emit plotsChanged(m_plotsVar);
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

QObject *AxisBase::app() const
{
    return m_app;
}

void AxisBase::setApp(QObject *arg)
{
    if (m_app == arg) return;
    m_app = arg;
    emit appChanged(arg);
}

void AxisBase::registerPlot(PlotBase *plot)
{
    QString key = plot->handle();

    if (m_plots.contains(key, plot))
        return; // Already in the list, move along now

    m_plots.insert(key, plot);
    updatePlots();
}

void AxisBase::deregisterPlot(PlotBase *plot)
{
    m_plots.remove(plot->handle(), plot);
    updatePlots();
}

