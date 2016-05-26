#include "nutmegobject.h"

#include <QObject>
#include <QQmlProperty>
#include <QDebug>

NutmegObject::NutmegObject(QObject *obj)
    : m_obj(obj)
    , m_metaParent(obj ? obj->metaObject() : 0)
    , m_childMethod(QMetaMethod())
    , m_props(QMap<QString, QMetaProperty>())
    , m_allmethods(QMap<QString, QMetaMethod>())
    , m_funcs(QMap<QString, QMetaMethod>())
    , m_propMap(QMap<QString, QString>())
    , m_funcMap(QMap<QString, QString>())
{
    if (!obj) {
        qWarning("WARNING: No QObject associated with NutmegObject...");
        return;
    }
}

void NutmegObject::registerChildMethod(const QString &name)
{
//    auto meta = m_obj->metaObject();
    m_childMethod = m_allmethods[name];
}

// TODO: Reimplement this method in FigureBase, AxisBase and GuiBase...
NutmegObject *NutmegObject::nutmegChild(const QString &name)
{
    if (!m_childMethod.isValid())
        return 0;

    NutmegObject *obj;
    bool succ = m_childMethod.invoke(m_obj, Qt::DirectConnection,
                                     Q_RETURN_ARG(NutmegObject*, obj), Q_ARG(QString, name));
    if (!succ)
        return 0;

    return obj;
}

void NutmegObject::registerProperty(const QString &handle)
{
    if (m_propMap.contains(handle))
        return;
    auto meta = m_obj->metaObject();

    m_propMap.insert(handle, handle);
    QMetaProperty prop = meta->property( meta->indexOfProperty(handle.toLatin1()) );
    m_props.insert(handle, prop);
}

void NutmegObject::registerProperties(QMap<QString, QString> mapping)
{
//    qDebug() << "RegisterProps" << mapping;
    auto meta = m_obj->metaObject();

    foreach (const QString key, mapping.keys()) {
        QString realname = mapping[key];
        m_propMap.insert(key, realname);

        QMetaProperty prop = meta->property( meta->indexOfProperty(realname.toLatin1()) );
        m_props.insert(key, prop);
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

QMetaProperty NutmegObject::property(const QString &handle)
{
    return m_props[handle];
}

void NutmegObject::registerMethod(const QString &handle)
{
    if (m_funcMap.contains(handle))
        return;

    if (m_allmethods.isEmpty())
        updateMethods();

    m_funcMap.insert(handle, handle);

    QMetaMethod func = m_allmethods.value(handle);
    m_funcs.insert(handle, func);
}

void NutmegObject::registerMethods(QMap<QString, QString> mapping)
{
    if (m_allmethods.isEmpty())
        updateMethods();

    foreach (const QString key, mapping.keys()) {
        QString realname = mapping[key];
        m_funcMap.insert(key, realname);

        QMetaMethod func = m_allmethods[realname];
        m_funcs.insert(key, func);
    }
}

QString NutmegObject::mapMethod(const QString &func) {
//    qDebug() << "NutmegObject.methodMap" << prop << m_funcMap.keys();
//    qDebug() << "mapping[prop]" << m_funcMap[prop] << m_funcMap.contains(prop);
    return m_funcMap[func];
}

QMetaMethod NutmegObject::method(const QString &handle)
{
    return m_funcs[handle];
}

void NutmegObject::updateMethods()
{
    if (!m_obj)
        return;
    auto meta = m_obj->metaObject();
    while (meta && meta != m_metaParent) {
        for(int i = meta->methodOffset(); i < meta->methodCount(); ++i) {
            QMetaMethod method = meta->method(i);
            m_allmethods[method.name()] = method;
        }
        meta = meta->superClass();
    }
}
