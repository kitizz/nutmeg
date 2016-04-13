#include "lineplot.h"
#include <QColor>
#include <QtGui/QGuiApplication>
#include <QWindow>

LinePlot::LinePlot(QQuickItem *parent)
    : XYPlot(parent)
    , m_points(QList<QPointF>())
    , m_line(new LineSpec())
{
    QMap<QString,QString> props;
    props.insert("x", "xData");
    props.insert("y", "yData");
    props.insert("line", "line");
    registerProperties(props);

}

LineSpec *LinePlot::line() const
{
    return m_line;
}

