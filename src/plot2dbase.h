#ifndef PLOT_H
#define PLOT_H

#include <QQuickItem>
#include <QPainter>
#include "plotdata.h"
#include "axis2dbase.h"
#include "nutmegobject.h"

#include "../QKDTree/QKDTree/QKDTree.h"

class Axis2DBase;
class Plot2DBase : public QQuickItem, public NutmegObject
{
    Q_OBJECT
    Q_PROPERTY(Axis2DBase* axis READ axis NOTIFY axisChanged)
    Q_PROPERTY(QString handle READ handle WRITE setHandle NOTIFY handleChanged)
    Q_PROPERTY(QQuickPaintedItem* canvas READ canvas WRITE setCanvas NOTIFY canvasChanged)

    Q_PROPERTY(QRectF dataLimits READ dataLimits NOTIFY dataLimitsChanged)

public:
    explicit Plot2DBase(QQuickItem *parent = 0);
    Axis2DBase* axis() const;

    void print(QPainter *painter);

    QString handle() const;
    void setHandle(QString arg);

    QQuickPaintedItem* canvas() const;
    void setCanvas(QQuickPaintedItem* arg);

    Q_INVOKABLE QPointF itemToData(QPointF point);
    Q_INVOKABLE QPointF dataToItem(QPointF point);

    Q_INVOKABLE QList<QPointF> itemToDataList(QList<QPointF> points);
    Q_INVOKABLE QList<QPointF> dataToItemList(QList<QPointF> points);

    QRectF dataLimits() const;

    // Nutmegobject API
    Q_INVOKABLE void registerProperties(QMap<QString, QString> mapping);
    Q_INVOKABLE void registerProperties(QVariantMap mapping);
    Q_INVOKABLE QString mapProperty(const QString &prop);
//    Q_INVOKABLE void registerMethods(QMap<QString, QString> mapping);
//    Q_INVOKABLE void registerMethods(QVariantMap mapping);
//    Q_INVOKABLE QString mapMethod(const QString &prop);

signals:
    void axisChanged(Axis2DBase* arg);
    void handleChanged(QString arg);
    void canvasChanged(QQuickPaintedItem* arg);
    void dataLimitsChanged(QRectF arg);

public slots:
    void triggerUpdate();
    void updateAxis();

protected slots:
    virtual void updateDataLimits() {}

protected:
    void setDataLimits(QRectF arg);
    QRectF m_dataLimits;

private:
    void setAxis(Axis2DBase* arg);

    Axis2DBase* m_axis;
    QString m_handle;
    QQuickPaintedItem* m_canvas;
};

#endif // PLOT_H
