#include "nutmegobject.h"

#include <QObject>
#include <QQmlProperty>
#include <QDebug>

void NutmegObject::registerProperties(QMap<QString, QString> mapping)
{
//    qDebug() << "RegisterProps" << mapping;
    foreach (const QString key, mapping.keys()) {
        m_propMap.insert(key, mapping[key]);
    }
}

void NutmegObject::registerProperties(QVariantMap mapping)
{
    QMap<QString, QString> map;
    foreach (QString tag, mapping.keys()) {
        QString prop = mapping.value(tag).toString();
        if (!prop.isEmpty())
            map.insert(tag, prop);
    }
    registerProperties(map);
}

QString NutmegObject::mapProperty(const QString &prop) {
//    qDebug() << "NutmegObject.map" << prop << m_propMap.keys();
//    qDebug() << "mapping[prop]" << m_propMap[prop] << m_propMap.contains(prop);
    return m_propMap[prop];
}

void NutmegObject::registerMethods(QMap<QString, QString> mapping)
{
    qDebug() << "RegisterMethods" << mapping;
    foreach (const QString key, mapping.keys()) {
        m_funcMap.insert(key, mapping[key]);
    }
}

QString NutmegObject::mapMethod(const QString &prop) {
//    qDebug() << "NutmegObject.methodMap" << prop << m_funcMap.keys();
//    qDebug() << "mapping[prop]" << m_funcMap[prop] << m_funcMap.contains(prop);
    return m_funcMap[prop];
}
