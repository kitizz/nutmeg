#ifndef LINEPLOT_H
#define LINEPLOT_H

#include "plotbase.h"
#include "axisbase.h"
#include <QPolygonF>
#include <QMap>

class LineSpec
{
public:
    LineSpec(QStringList specList);
    LineSpec(QString spec);

    bool isValid() const;
    QStringList toList() const;
    QString toString() const;

    QString color;
    QString style;
    QString marker;

    static QString markers() { return "+o*.xsd^v><ph"; }

    static QMap<QString, QColor> colorMap;
    static QMap<QString, Qt::PenStyle> styleMap;

private:
    static QMap<QString, QColor> generateColorMap() {
        QMap<QString, QColor> mapp = QMap<QString, QColor>();
        mapp.insert("r", Qt::red);
        mapp.insert("g", Qt::green);
        mapp.insert("b", Qt::blue);
        mapp.insert("c", Qt::cyan);
        mapp.insert("m", Qt::magenta);
        mapp.insert("y", Qt::yellow);
        mapp.insert("k", Qt::black);
        mapp.insert("w", Qt::white);
        return mapp;
    }

    static QMap<QString, Qt::PenStyle> generateStyleMap() {
        QMap<QString, Qt::PenStyle> mapp = QMap<QString, Qt::PenStyle>();
        mapp.insert("-", Qt::SolidLine);
        mapp.insert("--", Qt::DashLine);
        mapp.insert(":", Qt::DotLine);
        mapp.insert("-.", Qt::DashDotLine);
        return mapp;
    }
};

class LinePlot : public PlotBase
{
    Q_OBJECT
    Q_PROPERTY(QList<qreal> xData READ xData WRITE setXData NOTIFY xDataChanged)
    Q_PROPERTY(QList<qreal> yData READ yData WRITE setYData NOTIFY yDataChanged)
    Q_PROPERTY(QRectF dataLimits READ dataLimits NOTIFY dataLimitsChanged)

    Q_PROPERTY(QColor lineColor READ lineColor WRITE setLineColor NOTIFY lineColorChanged)
    Q_PROPERTY(QString lineStyle READ lineStyle WRITE setLineStyle NOTIFY lineStyleChanged)
    Q_PROPERTY(QString lineSpec READ lineSpec WRITE setLineSpec NOTIFY lineSpecChanged)
    Q_PROPERTY(qreal lineWidth READ lineWidth WRITE setLineWidth NOTIFY lineWidthChanged)
    Q_CLASSINFO("DefaultProperty", "lineSpec")

    Q_PROPERTY(QColor markerColor READ markerColor WRITE setMarkerColor NOTIFY markerColorChanged)
    Q_PROPERTY(QString markerStyle READ markerStyle WRITE setMarkerStyle NOTIFY markerStyleChanged)

public:
    explicit LinePlot(QQuickItem *parent = 0);

//    void paint(QPainter *painter);

    QList<qreal> xData() const;
    QList<qreal> yData() const;
    void setXData(QList<qreal> arg);
    void setYData(QList<qreal> arg);

    QRectF dataLimits() const;
    QColor lineColor() const;
    void setLineColor(QColor arg);

    QString lineStyle() const;
    void setLineStyle(const QString &style);

    QString lineSpec() const;
    void setLineSpec(const QString &arg);

    QColor markerColor() const;
    void setMarkerColor(QColor arg);

    QString markerStyle() const;
    void setMarkerStyle(const QString &arg);

    qreal lineWidth() const;
    void setLineWidth(qreal arg);

signals:
    void xDataChanged(QList<qreal> arg);
    void yDataChanged(QList<qreal> arg);

//    void widthChanged(qreal arg);
//    void heightChanged(qreal arg);

    void dataLimitsChanged(QRectF arg);
    void lineColorChanged(QColor arg);
    void lineStyleChanged(QString arg);
    void lineSpecChanged(QString arg);
    void markerColorChanged(QColor arg);
    void markerStyleChanged(QString arg);

    void lineWidthChanged(qreal arg);

public slots:
    void setData(QList<qreal> xVals, QList<qreal> yVals);

protected slots:
    void updateData();

private:
    bool m_settingData;
    QList<qreal> m_xData;
    QList<qreal> m_yData;
    QPointF m_dataMin;
    QPointF m_dataMax;
    QList<QPointF> m_line;

    qreal m_width;
    qreal m_height;
    QRectF m_dataLimits;
    QColor m_lineColor;
    QString m_lineStyle;
    LineSpec m_lineSpec;
    QColor m_markerColor;
    QString m_markerStyle;
    qreal m_lineWidth;
};

#endif // LINEPLOT_H
