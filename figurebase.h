#ifndef FIGURE_H
#define FIGURE_H

#include <QQuickItem>
#include <QMultiMap>
#include "axisbase.h"
#include "controller.h"
#include "nutmegobject.h"

class AxisBase;
class Controller;
class FigureBase : public QQuickItem, public NutmegObject
{
    Q_OBJECT
    Q_PROPERTY(QString handle READ handle WRITE setHandle NOTIFY handleChanged)
    Q_PROPERTY(QVariantMap axes READ axes NOTIFY axesChanged)
    Q_PROPERTY(Controller* controller READ controller WRITE setController NOTIFY controllerChanged)
    Q_PROPERTY(QObject* app READ app WRITE setApp NOTIFY appChanged)
public:
    explicit FigureBase(QQuickItem *parent = 0);
    ~FigureBase();

//    void paint(QPainter *painter);

    QVariantMap axes() const; // Use this for QML
    QList<AxisBase*> getAxesByHandle(const QString &handle) const; // Use this for c++

    QString handle() const;
    void setHandle(QString arg);

    Controller* controller() const;
    void setController(Controller* arg);

    Q_INVOKABLE QString map(QString prop);

    QObject* app() const;
    void setApp(QObject* arg);

signals:
    void axesChanged(QVariantMap arg);
    void handleChanged(QString arg);
    void addedAxis(AxisBase* axis);

    void controllerChanged(Controller* arg);

    void appChanged(QObject* arg);

public slots:
    void deregisterAxis(AxisBase *axis);
    void registerAxis(AxisBase *axis);

protected slots:
    void updateAxes();

private:
    QVariantMap m_axesVar;
    QMultiMap<QString,AxisBase*> m_axes;
    bool m_destroying;

    QString m_handle;
    Controller* m_controller;
    QObject* m_app;
};

#endif // FIGURE_H
