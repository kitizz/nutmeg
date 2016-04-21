#ifndef NUTMEGOBJECT_H
#define NUTMEGOBJECT_H

#include "../nutmeglibdecl.h"
#include <QMap>
#include <QString>
#include <QVariant>
#include <QObject>
#include <QMetaMethod>
#include <QMetaProperty>

class NUTMEGLIB_EXPORT NutmegObject
{
    Q_GADGET
public:
    NutmegObject(QObject *obj=0);

    virtual void registerChildMethod(const QString &name);
    Q_INVOKABLE virtual NutmegObject *nutmegChild(const QString &name);

    Q_INVOKABLE virtual void registerProperty(const QString &handle);
    Q_INVOKABLE virtual void registerProperties(QMap<QString, QString> mapping);
    Q_INVOKABLE virtual void registerProperties(QVariantMap mapping);
    Q_INVOKABLE virtual QString mapProperty(const QString &prop);
    virtual QMetaProperty property(const QString &handle);

    Q_INVOKABLE virtual void registerMethod(const QString &handle);
    Q_INVOKABLE virtual void registerMethods(QMap<QString, QString> mapping);
    Q_INVOKABLE virtual QString mapMethod(const QString &func);
    virtual QMetaMethod method(const QString &handle);

    QObject *object() { return m_obj; }

public slots:
    void updateMethods();

private:
    QObject *m_obj;
    const QMetaObject *m_metaParent;
    QMetaMethod m_childMethod;
    QMap<QString, QMetaProperty> m_props;
    QMap<QString, QMetaMethod> m_allmethods;
    QMap<QString, QMetaMethod> m_funcs;
    QMap<QString, QString> m_propMap;
    QMap<QString, QString> m_funcMap;
};

Q_DECLARE_METATYPE(NutmegObject)
Q_DECLARE_METATYPE(NutmegObject*)

#endif // NUTMEGOBJECT_H
