#ifndef GUIBASE_H
#define GUIBASE_H

#include <QQuickItem>
#include "nutmegobject.h"

class GuiItem;
class GuiBase : public QQuickItem, public NutmegObject
{
    Q_OBJECT

    Q_PROPERTY(QString figureHandle READ figureHandle WRITE setFigureHandle NOTIFY figureHandleChanged)

public:
    explicit GuiBase(QQuickItem *parent=0);
    ~GuiBase();

    Q_INVOKABLE GuiItem *parameter(const QString &handle);
    QList<QString> parameterList();

    QString figureHandle() const;
    void setFigureHandle(QString figureHandle);

    Q_INVOKABLE virtual NutmegObject *nutmegChild(const QString &name);

signals:
    void parameterChanged(QString figureHandle, QString parameter, qreal value);
    void figureHandleChanged(QString figureHandle);

public slots:
    void registerGuiItem(GuiItem *item);
    void deregisterGuiItem(GuiItem *item);

    void notifyParameterChanged(QString parameter, qreal value) {
        emit parameterChanged(figureHandle(), parameter, value);
    }

private:
    QString m_figureHandle;
    QMap<QString, GuiItem*> m_parameters;

};

#endif // GUIBASE_H
