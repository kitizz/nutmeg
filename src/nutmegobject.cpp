#include "nutmegobject.h"

#include <QObject>
#include <QQmlProperty>
#include <QDebug>

void NutmegObject::registerProperties(QMap<QString, QString> mapping)
{
    qDebug() << "RegisterProps" << mapping;
    foreach (const QString key, mapping.keys()) {
        m_mapping.insert(key, mapping[key]);
    }
}

QString NutmegObject::mapProperty(const QString &prop) {
    qDebug() << "NutmegObject.map" << prop << m_mapping.keys();
    qDebug() << "mapping[prop]" << m_mapping[prop] << m_mapping.contains(prop);
    return m_mapping[prop];
}
