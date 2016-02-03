#ifndef AXISBASE_H
#define AXISBASE_H

#include <QQuickItem>
#include <QString>
#include "nutmegobject.h"
#include "plotbase.h"

#include "layoutgrid.h"

class AxisGroup;
class FigureBase;
class PlotBase;
class LayoutGrid;
class AxisBase : public QQuickItem, public NutmegObject
{
    Q_OBJECT
    Q_PROPERTY(QString handle READ handle WRITE setHandle NOTIFY handleChanged)
    Q_PROPERTY(FigureBase* figure READ figure WRITE setFigure NOTIFY figureChanged)
    Q_PROPERTY(QRectF plotRect READ plotRect NOTIFY plotRectChanged)
    Q_PROPERTY(QRectF preferredPlotRect READ preferredPlotRect WRITE setPreferredPlotRect NOTIFY preferredPlotRectChanged)

    Q_PROPERTY(QVariantMap plots READ plots NOTIFY plotsChanged)

    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QFont titleFont READ titleFont WRITE setTitleFont NOTIFY titleFontChanged)
    Q_PROPERTY(QColor titleColor READ titleColor WRITE setTitleColor NOTIFY titleColorChanged)

public:
    explicit AxisBase(QQuickItem *parent = 0);
    ~AxisBase();

    // NutmegObject interface
//    Q_INVOKABLE void registerProperties(QMap<QString, QString> mapping);
//    Q_INVOKABLE void registerProperties(QVariantMap mapping);
//    Q_INVOKABLE QString mapProperty(const QString &prop);

    QVariantMap plots() const; // For QML
    PlotBase *getPlotByHandle(QString handle) const; // For C++
    Q_INVOKABLE QList<QString> plotNames() const;
    Q_INVOKABLE QVariantList getPlotList() const;

    QString handle() const;
    void setHandle(QString arg);

    FigureBase* figure() const;
    void setFigure(FigureBase* arg);

    QRectF plotRect() const;
    void setPlotRect(const QRectF &arg);
    void resetPlotRect();
    QRectF preferredPlotRect() const;
    void setPreferredPlotRect(const QRectF &arg);

    QString title() const;
    void setTitle(QString arg);

    QFont titleFont() const;
    void setTitleFont(QFont arg);

    QColor titleColor() const;
    void setTitleColor(QColor arg);

    void setLayoutGroup(LayoutGrid *layout);
    LayoutGrid *layoutGroup() const;
//    void setAxisGroupIndex(int row, int column);
//    void getAxisGroupIndex(int &row, int &column);

signals:
    void addedPlot(PlotBase* plot);
    void removedPlot(PlotBase* plot);
    void plotsChanged(QVariantMap arg);

    void handleChanged(QString arg);
    void figureChanged(FigureBase* arg);
    void plotRectChanged(QRectF arg);
    void preferredPlotRectChanged(QRectF arg);
    void registerWithFigure(AxisBase* axis);
    void titleChanged(QString arg);
    void titleFontChanged(QFont arg);
    void titleColorChanged(QColor arg);

public slots:
    void updateFigure();
    QRectF updatePlotRect();
    virtual void triggerRedraw() {}
    void registerPlot(PlotBase *plot);
    void deregisterPlot(PlotBase *plot);
    virtual void print(QPainter *painter) { Q_UNUSED(painter); }

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

protected slots:
    void updatePlots();

private:
    QString m_handle;
    FigureBase* m_figure;
    QRectF m_plotRect;
    QRectF m_preferredPlotRect;
    bool m_plotRectValid;
    QString m_title;
    QFont m_titleFont;
    QColor m_titleColor;

    QMap<QString, PlotBase*> m_plots;
    QVariantMap m_plotsVar;

    bool m_destroying;

    LayoutGrid *m_layoutGroup;
//    int m_groupRow, m_groupColumn;
};

#endif // AXISBASE_H
