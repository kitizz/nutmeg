#include "nutmegobject.h"

#include <QObject>
#include <QQmlProperty>
#include <QDebug>

void NutmegObject::registerProperties(QMap<QString, QString> mapping)
{
    foreach (const QString key, mapping.keys()) {
        m_mapping.insert(key, mapping[key]);
    }
}

QString NutmegObject::map(QString prop) {
    return m_mapping[prop];
}
