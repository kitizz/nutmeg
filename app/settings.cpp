#include "settings.h"

Settings::Settings(QObject *parent)
    : QObject(parent)
    , m_portSub(43686)
    , m_portPub(m_portSub + 1)
{
}

int Settings::portSubscribe() const
{
    return m_portSub;
}

void Settings::setPortSubscribe(int portSubscribe)
{
    if (m_portSub == portSubscribe)
        return;

    m_portSub = portSubscribe;
    emit portSubscribeChanged(portSubscribe);
}

int Settings::portPublish() const
{
    return m_portPub;
}

void Settings::setPortPublish(int portPublish)
{
    if (m_portPub == portPublish)
        return;

    m_portPub = portPublish;
    emit portPublishChanged(portPublish);
}
