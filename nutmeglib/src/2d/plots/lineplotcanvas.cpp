#include "lineplotcanvas.h"
#include "lineplot.h"

//TODO: Remove
#include <QTime>

#include "../../util/util.h"
#include "../../util/ndarray.h"

LinePlotCanvas::LinePlotCanvas(QQuickItem *parent) :
    PlotCanvas(parent)
{
}

void LinePlotCanvas::paint(QPainter *painter)
{
    if (!updateTriggered())
        return;
    resetTrigger();

    LinePlot *plot = qobject_cast<LinePlot*>(parent());
    if (!plot) return;

    Axis2DBase *monAxis = plot->axis2d();
    NDArray &xArray = plot->xData(),
            &yArray = plot->yData();

    bool emptyX = (xArray.size() == 0);
    int N = (emptyX) ? yArray.size() : qMin( xArray.size(), yArray.size() );

    if (!monAxis || N == 0 || yArray.type() == NDArray::Unknown)
        return; // Nothing to do

    NDArrayTyped<qreal> yData = yArray.convert<qreal>();

    // Allow the printer state to be restored
    painter->save();
    preparePainter(painter, plot);

    QRectF lim = monAxis->limits();

    QTransform tran = Util::plotToView(QSizeF(plot->width(), plot->height()), lim, monAxis->xAxis()->inverted(), monAxis->yAxis()->inverted());

    bool is_lines = (plot->line()->style() != ".");

    if (is_lines) {
        QPolygonF line;
        if (emptyX) {
            // Get the segments into screen coords
            int x = 0;
            auto y = yData.begin();
            for (int i=N; i; --i, ++x, ++y)
                line << QPointF(x, *y);

        } else {
            auto xData = xArray.convert<qreal>();
            // Get the segments into screen coords
            auto x = xData.begin(), y = yData.begin();
            for (int i=N; i; --i, ++x, ++y)
                line << QPointF(*x, *y);
        }

        // When the plot is zoomed closer, drawing the line as a single poly
        // gets slower - weird and annoying. Therefore, here, the lines are "cut"
        // by the view frame so that excess segment aren't drawn.
        QList<QPolygonF> lines;
        QPointF latestPoint;
        QPolygonF latestPoly;
        bool starting = true;

        // If there's only 1 point, the forloop will not run
        if (line.size() == 1)
            latestPoly << line[0];

        bool valid;
        for (int i=0; i<line.size() - 1; ++i) {
            QPointF p1 = line[i], p2 = line[i+1];
            QLineF l = rectSlice(p1, p2, lim, valid);
            if (l.isNull()) continue;

            // Check if the line has been broken, or is starting
            if (l.p1() != latestPoint || starting) {
                // Throw the current poly in and start a new one
                starting = false;
                lines << latestPoly;
                latestPoly = QPolygonF();
                latestPoly << l.p1();
            }
            latestPoly << l.p2();
            latestPoint = l.p2();
        }
        // Need the final whole section
        lines << latestPoly;

        foreach (QPolygonF poly, lines) {
            poly = tran.map(poly);
            painter->drawPolyline(poly);
        }

    } else {
        // Just drawing markers, therefore no need to "cut" lines, etc
        qreal radius = painter->pen().widthF() * 0.5;
        qreal tx, ty;

        if (emptyX) {
            int x = 0;
            auto y = yData.begin();
            for (int i = N; i; --i, ++x, ++y) {
                if (x < lim.left() || x > lim.right() || *y < lim.top() || *y > lim.bottom())
                    continue;

                tran.map(x, *y, &tx, &ty);
                painter->drawEllipse(tx, ty, radius, radius);
            }

        } else {
            auto xData = xArray.convert<qreal>();
            auto x = xData.begin(), y = yData.begin();
            for (int i = N; i; --i, ++x, ++y) {
                if (*x < lim.left() || *x > lim.right() || *y < lim.top() || *y > lim.bottom())
                    continue;

                tran.map(*x, *y, &tx, &ty);
                painter->drawEllipse(tx, ty, radius, radius);
            }
        }
    }

    // Restore the painter to its former glory
    painter->restore();
}

void LinePlotCanvas::updatePolish()
{

}

QPointF LinePlotCanvas::transformPoint(const QPointF &p, qreal tx, qreal ty, qreal sx, qreal sy, qreal limx, qreal limy)
{
    return QPointF((p.x() + tx)*sx - limx, (p.y() + ty)*sy - limy);
}

/*!
 * \brief LinePlotCanvas::rectSlice
 * Slice 2 points to a line contained by the rect, r
 * \param p1
 * \param p2
 * \param r
 * \return
 */
QLineF LinePlotCanvas::rectSlice(const QPointF &p1, const QPointF &p2, const QRectF &r, bool &valid)
{
    valid = true;
    // https://gist.github.com/ChickenProp/3194723
    qreal minX = r.left();
    qreal minY = r.top();
    qreal maxX = r.right();
    qreal maxY = r.bottom();

    qreal dx = p2.x() - p1.x();
    qreal dy = p2.y() - p1.y();

    qreal v[4] = { -dx, dx, -dy, dy };
    qreal u[4] = { p1.x() - minX, maxX - p1.x(), p1.y() - minY, maxY - p1.y() };
    qreal t[4];


    qreal tMax = Inf, tMin = -Inf;
    for (int i=0; i<4; ++i) {
        if (v[i] != 0) {
            t[i] = u[i]/v[i];
            if (v[i] < 0 && tMin < t[i])
                tMin = t[i];
            if (v[i] > 0 && tMax > t[i])
                tMax = t[i];

        } else if (u[i] >= 0) {
            return QLineF(p1, p2); // Inside rect
        } else {
            valid = false;
            return QLineF(); // Outside rect
        }
    }

    if (tMin >= tMax || tMax < 0 || tMin > 1) {
        valid = false;
        return QLineF();
    }

    if (tMax > 1) tMax = 1;
    if (tMax < 0) tMax = 0;
    if (tMin > 1) tMin = 1;
    if (tMin < 0) tMin = 0;

    QPointF q1(p1.x() + tMin*dx, p1.y() + tMin*dy);
    QPointF q2(p1.x() + tMax*dx, p1.y() + tMax*dy);

    return QLineF(q1, q2);
}

/**
 * @brief UtilEda::pointOnLine
 * @param point
 * @param l1 Defines the start of the line
 * @param l2 Defines the end of the line
 * @return Return 0 if the point is on the infinite line described by l1 and l2.
 * Return +ve if the point is "right" of the line - travelling from start to end.
 * Return -ve if the point is "left" of the line - travelling from start to end.
 */
qreal LinePlotCanvas::pointOnLine(QPointF point, QPointF l1, QPointF l2)
{
    return (l2.y() - l1.y())*point.x() + (l1.x() - l2.x())*point.y()
            + (l2.x()*l1.y() - l1.x()*l2.y());
}

QPen LinePlotCanvas::preparePainter(QPainter *painter, LinePlot* plot)
{
    painter->setRenderHint(QPainter::Antialiasing, plot->antialias());

    // Get the style right
    QPen pen = QPen();
    pen.setCosmetic(true);
    pen.setColor( plot->line()->color() );

    QString style = plot->line()->style();
    if (style == ".") {
        pen.setStyle(Qt::SolidLine);
    } else {
        pen.setStyle( LineSpec::styleMap[style] );
    }

    pen.setWidthF( plot->line()->width() );
    painter->setPen(pen);

    return pen;
}
