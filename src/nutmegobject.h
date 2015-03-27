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

    virtual void registerMethods(QMap<QString, QString> mapping);
    virtual QString mapMethod(const QString &method);

private:
    QMap<QString, QString> m_propMap;
    QMap<QString, QString> m_funcMap;
};

#endif // NUTMEGOBJECT_H
