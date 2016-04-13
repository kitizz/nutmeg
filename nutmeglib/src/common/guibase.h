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

    GuiItem *parameter(const QString &handle);

    QString figureHandle() const;
    void setFigureHandle(QString figureHandle);

signals:
    void parameterChanged(QString figureHandle, QString parameter, qreal value);
    void figureHandleChanged(QString figureHandle);

public slots:
    void registerGuiItem(GuiItem *item);
    void deregisterGuiItem(GuiItem *item);

private:
    QString m_figureHandle;
    QMap<QString, GuiItem*> m_parameters;

};

#endif // GUIBASE_H
