#include "lineplot.h"
#include <QColor>
#include <QtGui/QGuiApplication>
#include <QWindow>

QMap<QString, QColor> LineSpec::colorMap = LineSpec::generateColorMap();
QMap<QString, Qt::PenStyle> LineSpec::styleMap = LineSpec::generateStyleMap();

LinePlot::LinePlot(QQuickItem *parent)
    : PlotBase(parent)
    , m_points(QList<QPointF>())
    , m_line(new LineSpec())
{
    QMap<QString,QString> props;
    props.insert("x", "xData");
    props.insert("y", "yData");
    props.insert("lineColor", "lineColor");
    props.insert("lineStyle", "lineStyle");
    props.insert("lineSpec", "lineSpec");
    props.insert("markerColor", "markerColor");
    props.insert("markerStyle", "markerStyle");
    registerProperties(props);

}

LineSpec *LinePlot::line() const
{
    return m_line;
}

