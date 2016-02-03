#ifndef GUIITEM_H
#define GUIITEM_H

#include <QQuickItem>
#include "nutmegobject.h"

class GUIItem : public QQuickItem, public NutmegObject
{
    Q_OBJECT
public:
    explicit GUIItem(QQuickItem *parent = 0);

signals:

public slots:
    // Nutmeg object. TODO: Remove once Q_GADGET working properly...
    virtual void registerProperties(QMap<QString, QString> mapping) {
        NutmegObject::registerProperties(mapping);
    }
    virtual void registerProperties(QVariantMap mapping) {
        NutmegObject::registerProperties(mapping);
    }
    virtual QString mapProperty(const QString &prop) {
        return NutmegObject::mapProperty(prop);
    }

    virtual void registerMethods(QMap<QString, QString> mapping) {
        NutmegObject::registerMethods(mapping);
    }
    virtual QString mapMethod(const QString &method) {
        return NutmegObject::mapMethod(method);
    }

};

#endif // GUIITEM_H
