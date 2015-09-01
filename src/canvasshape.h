#ifndef CANVASSHAPE_H
#define CANVASSHAPE_H

#include <QQuickItem>
#include <QPainter>
#include "nutmegobject.h"

#include "linespec.h"
#include "fillspec.h"
#include "canvasplot.h"

class CanvasPlot;
class CanvasShape : public QQuickItem, public NutmegObject
{
    Q_OBJECT

    Q_PROPERTY(LineSpec* line READ lineSpec NOTIFY lineSpecChanged)
    Q_PROPERTY(FillSpec* fill READ fillSpec NOTIFY fillSpecChanged)

    Q_PROPERTY(CanvasPlot* plot READ plot NOTIFY plotChanged)

    Q_PROPERTY(QRectF bounds READ bounds NOTIFY boundsChanged)

public:
    explicit CanvasShape(QQuickItem *parent = 0);
    ~CanvasShape();

    virtual void paint(QPainter *painter, const QTransform &tran, const QRectF &limits) = 0;

    LineSpec* lineSpec() const;
    FillSpec* fillSpec() const;

    QRectF bounds() const;
    void setBounds(QRectF arg);

    CanvasPlot* plot() const;
    Q_INVOKABLE void setPlot(CanvasPlot* arg);

//    Q_INVOKABLE void registerMethods(QMap<QString, QString> mapping);
//    Q_INVOKABLE QString mapMethod(const QString &method);

signals:
    void lineSpecChanged(LineSpec* arg);
    void fillSpecChanged(FillSpec* arg);

    void boundsChanged(QRectF arg);

    void plotChanged(CanvasPlot* arg);

public slots:
    void updateDataLimits();
    void findPlot();

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

private:
    LineSpec* m_lineSpec;
    FillSpec* m_fillSpec;
    QRectF m_bounds;
    CanvasPlot* m_plot;

    bool m_settingBounds;

    bool m_destroying;
};

#endif // CANVASSHAPE_H
