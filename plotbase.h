#ifndef PLOT_H
#define PLOT_H

#include <QQuickItem>
#include <QPainter>
#include "plotdata.h"
#include "axisbase.h"
#include "nutmegobject.h"

class AxisBase;
class PlotBase : public QQuickItem, public NutmegObject
{
    Q_OBJECT
    Q_PROPERTY(QObject* app READ app WRITE setApp NOTIFY appChanged)
    Q_PROPERTY(AxisBase* axis READ axis NOTIFY axisChanged)
    Q_PROPERTY(QString handle READ handle WRITE setHandle NOTIFY handleChanged)
    Q_PROPERTY(QQuickPaintedItem* canvas READ canvas WRITE setCanvas NOTIFY canvasChanged)

public:
    explicit PlotBase(QQuickItem *parent = 0);
    AxisBase* axis() const;

    QString handle() const;
    void setHandle(QString arg);

    QQuickPaintedItem* canvas() const;
    void setCanvas(QQuickPaintedItem* arg);

    Q_INVOKABLE QString map(QString prop);

    QObject* app() const;
    void setApp(QObject* arg);

signals:
    void axisChanged(AxisBase* arg);
    void handleChanged(QString arg);
    void canvasChanged(QQuickPaintedItem* arg);

    void appChanged(QObject* arg);

public slots:
    void triggerUpdate();
    void updateAxis();

private:
    void setAxis(AxisBase* arg);

    AxisBase* m_axis;
    QString m_handle;
    QQuickPaintedItem* m_canvas;
    QObject* m_app;
};

#endif // PLOT_H
