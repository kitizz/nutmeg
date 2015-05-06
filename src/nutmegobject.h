#ifndef NUTMEGOBJECT_H
#define NUTMEGOBJECT_H

#include <QMap>
#include <QString>
#include <QVariant>
#include <QObject>

class NutmegObject
{
    Q_GADGET
public:
    Q_INVOKABLE virtual void registerProperties(QMap<QString, QString> mapping);
    Q_INVOKABLE virtual void registerProperties(QVariantMap mapping);
    Q_INVOKABLE virtual QString mapProperty(const QString &prop);

    Q_INVOKABLE virtual void registerMethods(QMap<QString, QString> mapping);
    Q_INVOKABLE virtual QString mapMethod(const QString &method);

private:
    QMap<QString, QString> m_propMap;
    QMap<QString, QString> m_funcMap;
};

#endif // NUTMEGOBJECT_H
