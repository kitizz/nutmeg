#include "guiitem.h"

#include "guibase.h"

GuiItem::GuiItem(QQuickItem *parent)
    : QQuickItem(parent)
    , NutmegObject(this)
    , m_handle(QString())
    , m_value(0)
    , m_gui(0)
{
    connect(this, &QQuickItem::parentChanged, this, &GuiItem::findGui);
    findGui();

    registerProperty("value");
    connect(this, &GuiItem::valueChanged, [=](qreal value){
        if (m_gui)
            m_gui->notifyParameterChanged(handle(), value);
    });
}

GuiItem::~GuiItem()
{
    if (m_gui)
        m_gui->deregisterGuiItem(this);
}

QString GuiItem::handle() const
{
    return m_handle;
}

void GuiItem::setHandle(QString handle)
{
    if (m_handle == handle)
        return;

    m_handle = handle;
    emit handleChanged(handle);
}

qreal GuiItem::value() const
{
    return m_value;
}

void GuiItem::setValue(qreal value)
{
    if (m_value == value)
        return;

    m_value = value;
    emit valueChanged(value);
}

GuiBase *GuiItem::gui() const
{
    return m_gui;
}

void GuiItem::setGui(GuiBase *gui)
{
    if (m_gui == gui)
        return;

    if (m_gui)
        m_gui->deregisterGuiItem(this);

    m_gui = gui;

    emit guiChanged(gui);

    if (m_gui) {
        m_gui->registerGuiItem(this);
        m_gui->notifyParameterChanged(m_handle, m_value);
    }
}

void GuiItem::findGui()
{
    if (m_gui)
        return;

//    // Disconnect from an external sender
//    QObject *sender = QObject::sender();
//    if (sender && this != sender)
//        sender->disconnect(this);

    // Work up the tree until the next Figure item is found.
    QQuickItem *newParent = parentItem();
    if (!newParent) return; // Either being deleted or instantiated
    GuiBase *guiObj;
    while (true) {
        guiObj = qobject_cast<GuiBase*>(newParent);
        if (guiObj)
            break;
        if (!newParent->parentItem()) {
            connect(newParent, &QQuickItem::parentChanged, this, &GuiItem::findGui);
            return;
        }
        newParent = newParent->parentItem();
    }

    if (guiObj) {
        disconnect(this, &QQuickItem::parentChanged, this, &GuiItem::findGui);
        setGui(guiObj);
    }
}

