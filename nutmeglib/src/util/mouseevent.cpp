#include "mouseevent.h"
#include <QMouseEvent>

MouseEvent::MouseEvent(qreal x, qreal y, Qt::MouseButton button, Qt::MouseButtons buttons, Qt::KeyboardModifiers keymod) :
    m_x(x), m_y(y),
    m_button(button),
    m_buttons(buttons),
    m_modifiers(keymod)
{
}

MouseEvent::MouseEvent(QEvent *event)
{
    QMouseEvent *m = (QMouseEvent*)event;
    m_x = m->x();
    m_y = m->y();
    m_button = m->button();
    m_buttons = m->buttons();
    m_modifiers = m->modifiers();
}

MouseEvent::MouseEvent(QObject *parent) :
    QObject(parent),
    m_x(0), m_y(0),
    m_button(0),
    m_buttons(0),
    m_modifiers(0)
{
}

int MouseEvent::button() const
{
    return m_button;
}

int MouseEvent::buttons() const
{
    return m_buttons;
}

int MouseEvent::modifiers() const
{
    return m_modifiers;
}

qreal MouseEvent::x() const
{
    return m_x;
}

qreal MouseEvent::y() const
{
    return m_y;
}
