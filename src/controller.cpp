#include "controller.h"

Controller::Controller(QQuickItem *parent)
    : QQuickItem(parent)
    , m_destroying(false)
    , m_figures(QMultiMap<QString,FigureBase*>())
    , m_figuresVar(QVariantMap())
{
}

Controller::~Controller()
{
    m_destroying = true;
}

QVariantMap Controller::figures() const
{
    return m_figuresVar;
}

QList<FigureBase *> Controller::getFiguresByHandle(const QString &handle)
{
    // TODO: This is O(n) complexity at the moment...
    // TODO: Abstract this pattern (also used in FigureBase and AxisBase)
    QList<FigureBase*> *values;
    if (handle.isEmpty())
        *values = m_figures.values();
    else
        *values = m_figures.values(handle);

    QList<FigureBase*> result;
    foreach (FigureBase* fig, *values)
        result.prepend(fig);

    return result;
}

void Controller::registerFigure(FigureBase *fig)
{
    QString key = fig->handle();
    if (m_figures.contains(key, fig))
        return; // Already in the list, move along now

    m_figures.insert(key, fig);
    updateFigures();
}

void Controller::deregisterFigure(FigureBase *fig)
{
    if (m_destroying) return; // Stops segfaults on close.
    QString key = fig->handle();
    m_figures.remove(key, fig);
    updateFigures();
}

void Controller::updateFigures()
{
    QVariantMap newMap; // Map of QVariantList
    foreach (const QString &key, m_figures.keys()) {
        QVariantList list;
        foreach (QObject *fig, m_figures.values(key))
            list.prepend(QVariant::fromValue(fig));
        newMap.insert(key, list);
    }
    m_figuresVar = newMap;
    emit figuresChanged(m_figuresVar);
}
