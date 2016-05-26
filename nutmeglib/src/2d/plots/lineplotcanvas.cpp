#include "lineplotcanvas.h"
#include "lineplot.h"

//TODO: Remove
#include <QTime>
#include <QPainterPath>

#include "../../util/util.h"
#include "../../util/ndarray.h"
#include <qmath.h>

/*!
 * \macro DRAW_LINES(xacc, yacc)
 * Draw lines into a QPainterPath. This exists to support having no x data, and auto filling
 * with sequential numbers from 0..N
 * The macro takes the specified accessors of locally defined variables x, y: xacc, yacc.
 * Uses the local variables that provide pointers to the data (x,y), plot-to-view transform
 * (tx, ty, sx, sy, lim), and a QPainterPath (path).
 */
#define DRAW_LINES(xacc, yacc) {\
    qreal x1 = xacc, y1 = yacc;\
    qreal x2, y2;\
    bool sliceEnd = true;\
    ++x; ++y;\
    \
    if (startx == 0 && starty == 0) {\
        for (int i = N - 1; i; --i, ++x, ++y) {\
            x2 = xacc; y2 = yacc;\
            Util::drawLineSlice(&path, x1, y1, x2, y2, tx, ty, sx, sy, lim, sliceEnd);\
            x1 = x2; y1 = y2;\
        }\
    } else {\
        int sxm1 = startx - 1, sym1 = starty - 1;\
        for (int i = N - 1; i; --i, x=(x+1) - (i==sxm1)*N, y=(y+1) - (i==sym1)*N) {\
            x2 = xacc; y2 = yacc;\
            Util::drawLineSlice(&path, x1, y1, x2, y2, tx, ty, sx, sy, lim, sliceEnd);\
            x1 = x2; y1 = y2;\
        }\
    }\
}

/*!
 * \macro DRAW_DOTS(xacc, yacc)
 * Similar to DRAW_LINES, but draws dots at the x and y locations.
 * \sa DRAW_LINES
 */
#define DRAW_DOTS(xacc, yacc) {\
    for (int i = N; i; --i, ++x, ++y) {\
        if (xacc < lim.left() || xacc > lim.right() || yacc < lim.top() || yacc > lim.bottom())\
            continue;\
    \
        tran.map(xacc, yacc, &px, &py);\
        painter->drawEllipse(px, py, radius, radius);\
    }\
}

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

    bool emptyX = (plot->xSize() == 0);
    int N = (emptyX) ? plot->ySize() : qMin( plot->xSize(), plot->ySize() );

    if (!monAxis || N == 0 || yArray.type() == NDArray::Unknown)
        return; // Nothing to do

    NDArrayTyped<qreal> yData = yArray.convert<qreal>();

    QRectF lim = monAxis->limits();

    // Get the transform params
    QTransform tran = Util::plotToView(QSizeF(plot->width(), plot->height()), lim, monAxis->xAxis()->inverted(), monAxis->yAxis()->inverted());
    qreal tx, ty, sx, sy, limx, limy;
    Util::plotToView(plot->width(), plot->height(), lim, monAxis->xAxis()->inverted(), monAxis->yAxis()->inverted(),
                    tx, ty, sx, sy);
    limx = lim.x();
    limy = lim.y();

    bool is_lines = (plot->line()->style() != ".");
    if (is_lines && N < 2)
        return; // Nothing to draw

    // Allow the printer state to be restored
    painter->save();
    preparePainter(painter, plot);

    if (is_lines) {
        QPainterPath path;
        int starty = plot->startIndexY();
        auto y = yData.begin() + starty;

        if (emptyX) {
            // Get the first point
            int startx = 0;
            qreal x = 0;
            DRAW_LINES(x, *y);

        } else {
            auto xData = xArray.convert<qreal>();
            // Get the first point
            int startx = plot->startIndexX();
            auto x = xData.begin() + startx;
            DRAW_LINES(*x, *y);
        }

//        qDebug() << "Path:" << path;
        painter->drawPath(path);

    } else {
        // Just drawing markers, therefore no need to "cut" lines, etc
        qreal radius = painter->pen().widthF() * 0.5;
        qreal px, py;

        if (emptyX) {
            int x = 0;
            auto y = yData.begin();
            DRAW_DOTS(x, *y);

        } else {
            auto xData = xArray.convert<qreal>();
            auto x = xData.begin(), y = yData.begin();
            DRAW_DOTS(*x, *y);
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

void LinePlotCanvas::transformPoint(qreal &x, qreal &y, qreal &x_out, qreal &y_out, qreal tx, qreal ty, qreal sx, qreal sy, qreal limx, qreal limy)
{
    x_out = (x - limx)*sx + tx;
    y_out = (y - limy)*sy + ty;
}

/*!
 * \brief LinePlotCanvas::rectSlice
 * Slice 2 points to a line contained by the rect, r
 * \param p1
 * \param p2
 * \param r
 * \return
 */
void LinePlotCanvas::rectSlice(qreal &x1, qreal &y1, qreal &x2, qreal &y2, const QRectF &r, bool &valid, bool &sliceStart, bool &sliceEnd)
{
    valid = true;
    // https://gist.github.com/ChickenProp/3194723

    qreal dx = x2 - x1;
    qreal dy = y2 - y1;

    qreal v[4] = { -dx, dx, -dy, dy };
    qreal u[4] = { x1 - r.left(), r.right() - x1, y1 - r.top(), r.bottom() - y1 };
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
            sliceStart = false;
            sliceEnd = false;
            return; // Inside rect
        } else {
            valid = false;
            return; // Outside rect
        }
    }

    if (tMin >= tMax || tMax < 0 || tMin > 1) {
        valid = false;
        return;
    }

    tMax = qMax(0.0, qMin(tMax, 1.0));
    tMin = qMax(0.0, qMin(tMin, 1.0));

    sliceStart = tMin > 0;
    sliceEnd = tMax < 1;

    x2 = x1 + tMax*dx;
    x1 += tMin*dx;
    y2 = y1 + tMax*dy;
    y1 += tMin*dy;
}

//void LinePlotCanvas::drawLineSlice(QPainterPath *path, qreal x1, qreal y1, qreal x2, qreal y2, qreal tx, qreal ty, qreal sx, qreal sy, const QRectF &lim, bool &sliceEnd)
//{
//    // https://gist.github.com/ChickenProp/3194723

//    qreal dx = x2 - x1;
//    qreal dy = y2 - y1;

//    qreal limx = lim.x(), limy = lim.y();
//    qreal v[4] = { -dx, dx, -dy, dy };
//    qreal u[4] = { x1 - limx, lim.right() - x1, y1 - limy, lim.bottom() - y1 };
//    qreal t[4];

//    qreal tMax = Inf, tMin = -Inf;
//    bool slice = true;
//    for (int i=0; i<4; ++i) {
//        if (v[i] != 0) {
//            t[i] = u[i]/v[i];
//            if (v[i] < 0 && tMin < t[i])
//                tMin = t[i];
//            if (v[i] > 0 && tMax > t[i])
//                tMax = t[i];

//        } else if (u[i] >= 0) {
//            // Line is fully inside rect
//            slice = false;
//            break;
//        } else {
//            return; // Outside rect
//        }
//    }

//    if (slice) {
//        // Check if valid line
//        if (tMin >= tMax || tMax < 0 || tMin > 1){
//            return;
//        }

//        tMax = qMax(0.0, qMin(tMax, 1.0));
//        tMin = qMax(0.0, qMin(tMin, 1.0));

//        x2 = x1 + tMax*dx;
//        x1 += tMin*dx;
//        y2 = y1 + tMax*dy;
//        y1 += tMin*dy;
//    }

//    // Transform into screen coords
//    x1 = (x1 - limx)*sx + tx;
//    x2 = (x2 - limx)*sx + tx;
//    y1 = (y1 - limy)*sy + ty;
//    y2 = (y2 - limy)*sy + ty;
//    if (sliceEnd)
//        path->moveTo(x1, y1);
//    path->lineTo(x2, y2);
//    sliceEnd = tMax < 1;
////    painter->drawLine(x1, y1, x2, y2);
//}

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
