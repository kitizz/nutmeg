#include "figurebase.h"

#include <QVariant>

FigureBase::FigureBase(QQuickItem *parent)
    : QQuickItem(parent)
    , m_destroying(false)
    , m_handle(QString())
    , m_controller(0)
{
    m_axes = QMultiMap<QString,AxisBase*>();
    m_axesVar = QVariantMap();

    QMap<QString,QString> props;
    registerProperties(props);
}

FigureBase::~FigureBase()
{
    m_controller->deregisterFigure(this);
    m_destroying = true;
}

//void FigureBase::paint(QPainter *painter)
//{

//}

QVariantMap FigureBase::axes() const
{
    return m_axesVar;
}

/**
 * @brief FigureBase::getAxesByHandle
 * @param handle The handle of the axes you wish to access.
 * @return A list of the axes with the requested handle if handle is not empty,
 *      else all the axes belonging to the figure.
 */
QList<AxisBase*> FigureBase::getAxesByHandle(const QString &handle) const
{
    // TODO: This is O(n) complexity at the moment...
    QList<AxisBase*> values;
    if (handle.isEmpty())
        values = m_axes.values();
    else
        values = m_axes.values(handle);

    QList<AxisBase*> result;
    foreach (AxisBase* ax, values)
        result.prepend(ax);

    return result;
}

void FigureBase::registerAxis(AxisBase *axis)
{
    QString key = axis->handle();
    qDebug() << "FigureBase::registerAxis" << key << axis;
    if (m_axes.contains(key, axis))
        return; // Already in the list, move along now

    m_axes.insert(key, axis);
    updateAxes();
}

void FigureBase::deregisterAxis(AxisBase *axis)
{
    if (m_destroying) return; // Stops segfaults on close.
    QString key = axis->handle();
    m_axes.remove(key, axis);
    updateAxes();
}

QString FigureBase::handle() const
{
    return m_handle;
}

void FigureBase::setHandle(QString arg)
{
    if (m_handle == arg) return;
    m_handle = arg;
    emit handleChanged(arg);
}

Controller *FigureBase::controller() const
{
    return m_controller;
}

void FigureBase::setController(Controller *arg)
{
    qDebug() << "setController";
    if (m_controller == arg) return;

    if (m_controller)
        m_controller->deregisterFigure(this);

    m_controller = arg;

    if (m_controller)
        m_controller->registerFigure(this);

    emit controllerChanged(arg);
}

QString FigureBase::map(QString prop)
{
    return NutmegObject::map(prop);
}

QObject *FigureBase::app() const
{
    return m_app;
}

void FigureBase::setApp(QObject *arg)
{
    if (m_app == arg) return;
    m_app = arg;
    emit appChanged(arg);
}

void FigureBase::updateAxes()
{
    QVariantMap newMap; // Map of QVariantList
    foreach (const QString &key, m_axes.keys()) {
        QVariantList list;
        foreach (QObject *ax, m_axes.values(key))
            list.prepend(QVariant::fromValue(ax));
        newMap.insert(key, list);
    }
    m_axesVar = newMap;
    emit axesChanged(m_axesVar);
}
