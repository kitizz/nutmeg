#include "settings.h"

Settings::Settings(QObject *parent)
    : QObject(parent)
    , m_port(43686)
{
}

int Settings::port() const
{
    return m_port;
}

void Settings::setPort(int arg)
{
    if (m_port == arg) return;
    m_port = arg;
    emit portChanged(arg);
}
