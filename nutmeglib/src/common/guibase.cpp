#include "guibase.h"
#include "guiitem.h"

GuiBase::GuiBase(QQuickItem *parent)
    : QQuickItem(parent)
    , NutmegObject(this)
{
    registerChildMethod("parameter");
}

GuiBase::~GuiBase()
{
    foreach (auto item, m_parameters.values())
        item->setGui(0);
}

GuiItem *GuiBase::parameter(const QString &handle)
{
    return m_parameters.value(handle, 0);
}

QList<QString> GuiBase::parameterList()
{
    return m_parameters.keys();
}

QString GuiBase::figureHandle() const
{
    return m_figureHandle;
}

void GuiBase::setFigureHandle(QString figureHandle)
{
    if (m_figureHandle == figureHandle)
        return;

    m_figureHandle = figureHandle;
    emit figureHandleChanged(figureHandle);
}

NutmegObject *GuiBase::nutmegChild(const QString &name)
{
    return parameter(name);
}

void GuiBase::registerGuiItem(GuiItem *item)
{
    QString key = item->handle();

    if (m_parameters.contains(key))
        return;

    m_parameters.insert(key, item);
}

void GuiBase::deregisterGuiItem(GuiItem *item)
{
    m_parameters.remove(item->handle());
}
