#ifndef AXIS_H
#define AXIS_H

#include <QQuickPaintedItem>
#include <QMultiMap>

#include "figurebase.h"
#include "plotbase.h"
#include "nutmegobject.h"

#include <limits>
#define Inf std::numeric_limits<double>::infinity()

class FigureBase;
class PlotBase;
class AxisBase : public QQuickPaintedItem, public NutmegObject
{
    Q_OBJECT
    Q_PROPERTY(QObject* app READ app WRITE setApp NOTIFY appChanged)
    Q_PROPERTY(QString handle READ handle WRITE setHandle NOTIFY handleChanged)

    Q_PROPERTY(FigureBase* figure READ figure WRITE setFigure NOTIFY figureChanged)
    Q_PROPERTY(QVariantMap plots READ plots NOTIFY plotsChanged)

    Q_PROPERTY(qreal minX READ minX WRITE setMinX NOTIFY minXChanged)
    Q_PROPERTY(qreal maxX READ maxX WRITE setMaxX NOTIFY maxXChanged)
    Q_PROPERTY(qreal minY READ minY WRITE setMinY NOTIFY minYChanged)
    Q_PROPERTY(qreal maxY READ maxY WRITE setMaxY NOTIFY maxYChanged)

public:
    explicit AxisBase(QQuickItem *parent = 0);
    ~AxisBase();

    void paint(QPainter *painter);

    FigureBase* figure() const;

    qreal minX() const;
    void setMinX(qreal arg);
    qreal maxX() const;
    void setMaxX(qreal arg);
    qreal minY() const;
    void setMinY(qreal arg);
    qreal maxY() const;
    void setMaxY(qreal arg);

    QString handle() const;
    void setHandle(QString arg);

    QVariantMap plots(); // For QML
    QList<PlotBase*> getPlotsByHandle(QString handle); // For C++

    // NutmegObject interface
    Q_INVOKABLE QString map(QString prop);

    QObject* app() const;
    void setApp(QObject* arg);

signals:
    void figureChanged(FigureBase* arg);
    void registerWithFigure(AxisBase* axis);
    void handleChanged(QString arg);
    void plotsChanged(QVariantMap arg);

    void minXChanged(qreal arg);
    void maxXChanged(qreal arg);
    void minYChanged(qreal arg);
    void maxYChanged(qreal arg);

    void appChanged(QObject* arg);

public slots:
    void registerPlot(PlotBase *axis);
    void deregisterPlot(PlotBase *axis);
    void setFigure(FigureBase* arg);

protected slots:
    void updateFigure();
    void updatePlots();

private:

    FigureBase* m_figure;
    QMultiMap<QString, PlotBase*> m_plots;
    QVariantMap m_plotsVar;

    QString m_handle;
    qreal m_minX;
    qreal m_maxX;
    qreal m_minY;
    qreal m_maxY;

    bool m_destroying;
    QObject* m_app;
};

#endif // AXIS_H
