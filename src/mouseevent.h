#ifndef MOUSEEVENT_H
#define MOUSEEVENT_H

#include <QObject>

class MouseEvent : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal x READ x)
    Q_PROPERTY(qreal y READ y)
    Q_PROPERTY(int button READ button)
    Q_PROPERTY(int buttons READ buttons)
    Q_PROPERTY(int modifiers READ modifiers)

public:
    explicit MouseEvent(QObject *parent=0);
    explicit MouseEvent(QEvent *event);
    explicit MouseEvent(qreal x, qreal y,Qt::MouseButton button, Qt::MouseButtons buttons, Qt::KeyboardModifiers keymod);

int button() const;
int buttons() const;
int modifiers() const;

qreal x() const;
qreal y() const;
signals:
public slots:

private:
    qreal m_x;
    qreal m_y;
    int m_button;
    int m_buttons;
    int m_modifiers;

};

#endif // MOUSEEVENT_H
