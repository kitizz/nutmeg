#include "fillspec.h"

FillSpec::FillSpec(QObject *parent)
    : QObject(parent)
    , m_color(Qt::white)
    , m_alpha(1.0)
{
    QMap<QString, QString> props;
    props.insert("color", "color");
    NutmegObject::registerProperties(props);
}

QColor FillSpec::color() const
{
    return m_color;
}

void FillSpec::setColor(QColor arg)
{
    if (m_color == arg) return;
    m_color = arg;
    emit colorChanged(arg);
}

qreal FillSpec::alpha() const
{
    return m_alpha;
}

void FillSpec::setAlpha(qreal arg)
{
    // Clamp the value before testing...
    if (arg > 1) arg = 1;
    else if (arg < 0) arg = 0;
    if (m_alpha == arg) return;
    m_alpha = arg;
    emit alphaChanged(arg);
}

QString FillSpec::mapProperty(const QString &prop)
{
    return NutmegObject::mapProperty(prop);
}
