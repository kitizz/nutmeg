#ifndef GUIITEM_H
#define GUIITEM_H

#include <QQuickItem>
#include "nutmegobject.h"

class GuiBase;
class NUTMEGLIB_EXPORT GuiItem : public QQuickItem, public NutmegObject
{
    Q_OBJECT
    Q_PROPERTY(QString handle READ handle WRITE setHandle NOTIFY handleChanged)
    Q_PROPERTY(qreal value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(QString figureHandle READ figureHandle WRITE setFigureHandle NOTIFY figureHandleChanged)
    Q_PROPERTY(GuiBase *gui READ gui WRITE setGui NOTIFY guiChanged)

public:
    explicit GuiItem(QQuickItem *parent = 0);
    ~GuiItem();

    QString figureHandle() const;
    void setFigureHandle(const QString &figureHandle);

    QString handle() const;
    void setHandle(QString handle);

    qreal value() const;
    void setValue(qreal value);

    GuiBase *gui() const;
    void setGui(GuiBase *gui);

signals:
    void figureHandleChanged(QString figureHandle);
    void handleChanged(QString handle);
    void valueChanged(qreal value);
    void guiChanged(GuiBase * gui);

public slots:
    void findGui();
    // Nutmeg object.
    // TODO: Remove once Q_GADGET working properly...
//    virtual void registerProperties(QMap<QString, QString> mapping) {
//        NutmegObject::registerProperties(mapping);
//    }
//    virtual void registerProperties(QVariantMap mapping) {
//        NutmegObject::registerProperties(mapping);
//    }
//    virtual QString mapProperty(const QString &prop) {
//        return NutmegObject::mapProperty(prop);
//    }

//    virtual void registerMethods(QMap<QString, QString> mapping) {
//        NutmegObject::registerMethods(mapping);
//    }
//    virtual QString mapMethod(const QString &method) {
//        return NutmegObject::mapMethod(method);
//    }


private:
    QString m_figureHandle;
    QString m_handle;
    qreal m_value;
    GuiBase * m_gui;
};

#endif // GUIITEM_H
