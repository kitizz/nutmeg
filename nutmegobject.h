#ifndef NUTMEGOBJECT_H
#define NUTMEGOBJECT_H

#include <QMap>
#include <QString>
#include <QVariant>
#include <QObject>

class NutmegObject
{
public:
    Q_INVOKABLE void registerProperties(QMap<QString, QString> mapping);
    Q_INVOKABLE QString map(QString prop);

private:
    QMap<QString, QString> m_mapping;
};

#endif // NUTMEGOBJECT_H
