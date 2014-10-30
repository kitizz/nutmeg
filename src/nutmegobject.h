#ifndef NUTMEGOBJECT_H
#define NUTMEGOBJECT_H

#include <QMap>
#include <QString>
#include <QVariant>
#include <QObject>

class NutmegObject
{
public:
    virtual void registerProperties(QMap<QString, QString> mapping);
    virtual QString mapProperty(const QString &prop);

private:
    QMap<QString, QString> m_mapping;
};

#endif // NUTMEGOBJECT_H
