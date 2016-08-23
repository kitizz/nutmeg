#include "lineplot.h"
#include <QColor>
#include <QtGui/QGuiApplication>
#include <QWindow>

#include "lineplotrenderer.h"

LinePlot::LinePlot(QQuickItem *parent)
    : XYPlot(parent)
    , m_renderer(0)
    , m_points(QList<QPointF>())
    , m_line(new LineSpec())
{
    QMap<QString,QString> props;
    props.insert("x", "xData");
    props.insert("y", "yData");
    props.insert("line", "line");
    registerProperties(props);

}

LinePlot::~LinePlot()
{
    if (m_renderer)
        delete m_renderer;
    delete m_line;
}

LineSpec *LinePlot::line() const
{
    return m_line;
}

NutmegRenderer *LinePlot::renderer()
{
    if (!m_renderer) {
        m_renderer = new LinePlotRenderer(this);
        m_renderer->initialize();
        connect(this, &PlotBase::dataChanged, [=](){ m_renderer->dataChanged(); });
    }
    return m_renderer;
}

