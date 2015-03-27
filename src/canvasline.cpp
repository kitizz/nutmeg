#include "canvasline.h"

#include "axis2dbase.h"

#include "util.h"

CanvasLine::CanvasLine(QQuickItem *parent) :
    CanvasShape(parent)
{
    QMap<QString, QString> props;
    props.insert("x", "xData");
    props.insert("y", "yData");
    registerProperties(props);

    // Tie the fill color to the line color (for legends?)
//    connect(lineSpec(), &LineSpec::colorChanged, [=](const QColor &c){ fillSpec()->setColor(c);});
}

void CanvasLine::paint(QPainter *painter, const QTransform &tran, const QRectF &limits)
{
    // TODO: This boilerplate style stuff to a CanvasShape::applyStyle(QPainter*) func?
    QPen pen;
    LineSpec *ln = lineSpec();
    pen.setColor(ln->color());
    pen.setWidthF(ln->width());

    QString style = ln->style();
    if (style == ".") {
        pen.setStyle(Qt::SolidLine);
    } else {
        pen.setStyle( LineSpec::styleMap[style] );
    }

    painter->setBrush(Qt::NoBrush);
    painter->setPen(pen);
    painter->setRenderHint(QPainter::Antialiasing);

    // This stuff is the juicy stuff...
    QPolygonF line;
    // Get the segments into screen coords
    for (int i=0; i<m_yData.length(); ++i)
        line << QPointF(m_xData[i], m_yData[i]);

    QList<QPolygonF> lines = Util::cullLine(line, limits);

    qreal scaleX = tran.m11();
    qreal scaleY = tran.m22();

    foreach (QPolygonF poly, lines) {
        poly = tran.map(poly);
        if (style == ".") {
            foreach (QPointF p, poly) {
                painter->drawEllipse(p, pen.widthF()/scaleX, pen.widthF()/scaleY);
            }
        } else {
            painter->drawPolyline(poly);
        }
    }
}

QList<qreal> CanvasLine::xData() const
{
    return m_xData;
}

void CanvasLine::setXData(QList<qreal> arg)
{
    if (m_xData == arg) return;
    m_xData = arg;
    updateDataLimits();
    emit xDataChanged(arg);
}

QList<qreal> CanvasLine::yData() const
{
    return m_yData;
}

void CanvasLine::setYData(QList<qreal> arg)
{
    if (m_yData == arg) return;
    m_yData = arg;
    updateDataLimits();
    emit yDataChanged(arg);
}

void CanvasLine::updateDataLimits()
{
    int xLen = m_xData.length(), yLen = m_yData.length();
    if ((xLen > 0 && xLen != yLen))
        return; // Don't update until the data is good

    qreal minX = Inf, maxX = -Inf, minY = Inf, maxY = -Inf;
    for (int i=0; i<yLen; ++i) {
        qreal px = m_xData[i];
        qreal py = m_yData[i];
        minX = qMin(minX, px);
        maxX = qMax(maxX, px);
        minY = qMin(minY, py);
        maxY = qMax(maxY, py);
    }

    QRectF newLim;
    newLim.setLeft(minX);
    newLim.setTop(minY);
    newLim.setRight(maxX);
    newLim.setBottom(maxY);
    setBounds(newLim);
}
