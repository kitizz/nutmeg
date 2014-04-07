#include "plotbase.h"
#include "axisbase.h"
#include <QPolygonF>
#include <QPainter>

PlotBase::PlotBase(QQuickItem *parent)
    : QQuickItem(parent)
    , m_axis(0)
    , m_handle(QString())
{
    connect(this, &QQuickItem::parentChanged, this, &PlotBase::updateAxis);
}

AxisBase *PlotBase::axis() const
{
    return m_axis;
}

QString PlotBase::handle() const
{
    return m_handle;
}

void PlotBase::setHandle(QString arg)
{
    if (m_handle == arg) return;
    m_handle = arg;
    emit handleChanged(arg);
}

QQuickPaintedItem *PlotBase::canvas() const
{
    return m_canvas;
}

void PlotBase::setCanvas(QQuickPaintedItem *arg)
{
    if (m_canvas == arg) return;
    m_canvas = arg;
    emit canvasChanged(arg);
}

QString PlotBase::map(QString prop)
{
    return NutmegObject::map(prop);
}

QObject *PlotBase::app() const
{
    return m_app;
}

void PlotBase::setApp(QObject *arg)
{
    if (m_app == arg) return;
    m_app = arg;
    emit appChanged(arg);
}

void PlotBase::setAxis(AxisBase *arg)
{
    if (m_axis == arg) return;

    if (m_axis) {
        m_axis->disconnect(this);
        m_axis->deregisterPlot(this);
    }

    m_axis = arg;

    if (m_axis) {
        connect(m_axis, SIGNAL(minXChanged(qreal)), this, SLOT(triggerUpdate()));
        connect(m_axis, SIGNAL(minYChanged(qreal)), this, SLOT(triggerUpdate()));
        connect(m_axis, SIGNAL(maxXChanged(qreal)), this, SLOT(triggerUpdate()));
        connect(m_axis, SIGNAL(maxYChanged(qreal)), this, SLOT(triggerUpdate()));
        m_axis->registerPlot(this);
    } else {
        qWarning() << Q_FUNC_INFO << "Plot is not a descendant of any Axis. It may not behave as expected.";
    }
    emit axisChanged(arg);
}

void PlotBase::triggerUpdate()
{
    update();
    if (m_canvas)
        m_canvas->update();
}

void PlotBase::updateAxis()
{
    // Work up the tree until the next Axis item is found.
    QObject *newParent = parent();
    AxisBase *axis;
    while (newParent) {
        axis = qobject_cast<AxisBase*>(newParent);
        if (axis) break;
        newParent = newParent->parent();
    }

    setAxis(axis);
}
