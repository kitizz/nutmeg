#include "util.h"
#include <QtCore/qmath.h>
#include <cmath>
#include <stdlib.h>

#include <QMap>
#include <QDebug>
#include <QPainter>

int Util::randInt(int low, int high) {
    // From: http://goo.gl/KdHeUg
    return qrand() % ((high + 1) - low) + low;
}

QList<int>* Util::randomArray(int N)
{
    // http://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle
    QList<int> *lst = new QList<int>();
    lst->reserve(N);

    for (int i=0; i<N; ++i)
        (*lst)[i] = i;

    for (int i=0; i<N; ++i) {
        int n = randInt(i, N-1);
        lst->swap(i, n);
    }

    return lst;
}

qreal Util::median(QList<qreal> lst)
{
    // Assumes already sorted
    int N = lst.length();
    if (N % 2 == 1) {
        return lst[N/2];
    } else {
        return 0.5*(lst[N/2 - 1] + lst[N/2]);
    }
}

int Util::mode(QList<int> lst)
{
    QMap<int,int> map;
    foreach (int val, lst)
        map[val] += 1;

    int maxCount = 0, maxVal = 0;
    foreach (int val, map.keys()) {
        int count = map[val];
        if (count > maxCount) {
            maxCount = count;
            maxVal = val;
        }
    }
    return maxVal;
}

int Util::min(QList<int> lst)
{
    if (lst.length() == 0) return 0;

    int result = lst[0];
    foreach (int v, lst) {
        if (v < result)
            result = v;
    }
    return result;
}

int Util::max(QList<int> lst)
{
    if (lst.length() == 0) return 0;

    int result = lst[0];
    foreach (int v, lst) {
        if (v > result)
            result = v;
    }
    return result;
}

QList<qreal> Util::diff(QList<qreal> lst)
{
    QList<qreal> result;
    for (int i=1; i<lst.length(); ++i) {
        result << (lst[i] - lst[i-1]);
    }
    return result;
}

qreal Util::roundTo(qreal value, int precision)
{
    qreal scale = qPow(10, precision);
    return scale*(qRound(value/scale));
}

/*!
 * \fn resizeRelativeRatio
 * Resize \a rect so that the ratio,
 * rect.width/size.width : rect.height/size.height,
 * is equal to \a aspectRatio. Where aspectRatio = width/height.
 * \a anchor selects whether width or height is to be kept the same in \a rect.
 * \a center is a QPointF of numbers between 0 and 1. 0 centers the resize to
 * the left, 1 centers the resize to the right.
 */
void Util::resizeRelativeRatio(QRectF *rect, QSizeF size, qreal aspectRatio, AnchorSide anchor, QPointF center)
{
    if (aspectRatio <= 0) return;

    if (anchor == AnchorFit) {
        anchor = rect->width()/size.width() > rect->height()/size.height()
                ? AnchorWidth : AnchorHeight;
    }

    if (anchor == AnchorWidth) {
        qreal ratio = rect->width()/size.width();
        qreal newHeight = ratio*size.height()/aspectRatio;
        rect->setY(rect->y() - center.y()*(newHeight - rect->height()));
        rect->setHeight(newHeight);

    } else if (anchor == AnchorHeight) {
        qreal ratio = rect->height()/size.height();
        qreal newWidth = ratio*size.width()/aspectRatio;
        rect->setX(rect->x() - center.x()*(newWidth - rect->width()));
        rect->setWidth(newWidth);
    }
}

/*!
 * \fn cullLine
 * Slice \a poly so that the segments that are left are those which remain inside \a rect.
 */
QList<QPolygonF> Util::cullLine(QPolygonF poly, QRectF rect)
{
    // When the plot is zoomed closer, drawing the line as a single poly
    // gets slower - weird and annoying. Therefore, here, the lines are "cut"
    // by the view frame so that excess segment aren't drawn.
    QList<QPolygonF> lines;
    QPointF latestPoint;
    QPolygonF latestPoly;
    bool starting = true;

    // If there's only 1 point, the forloop will not run
    if (poly.size() == 1) {
        if (rect.contains(poly[0])) {
            latestPoly << poly[0];
            lines << latestPoly;
        }
        return lines;
    }

    for (int i=0; i<poly.size() - 1; ++i) {
        QPointF p1 = poly[i], p2 = poly[i+1];
        QLineF l = rectSlice(p1, p2, rect);
        if (l.isNull()) continue;

        // Check if the line has been broken, or is starting
        if (l.p1() != latestPoint || starting) {
            // Throw the current poly in and start a new one
            if (latestPoly.length() > 0) {
                lines << latestPoly;
                latestPoly = QPolygonF();
            }
            starting = false;
            latestPoly << l.p1();
        }
        latestPoly << l.p2();
        latestPoint = l.p2();
    }

    // Need the final whole section
    lines << latestPoly;

    return lines;
}

QLineF Util::rectSlice(QPointF p1, QPointF p2, QRectF r)
{
    // https://gist.github.com/ChickenProp/3194723
    qreal minX = r.x();
    qreal minY = r.y();
    qreal maxX = minX + r.width();
    qreal maxY = minY + r.height();

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
            return QLineF(); // Outside rect
        }
    }

    if (tMin >= tMax)
        return QLineF();

    if (tMax > 1) tMax = 1;
    if (tMax < 0) tMax = 0;
    if (tMin > 1) tMin = 1;
    if (tMin < 0) tMin = 0;

    QPointF q1(p1.x() + tMin*dx, p1.y() + tMin*dy);
    QPointF q2(p1.x() + tMax*dx, p1.y() + tMax*dy);

    return QLineF(q1, q2);
}

void Util::drawLineSlice(QPainterPath *path, qreal x1, qreal y1, qreal x2, qreal y2, qreal tx, qreal ty, qreal sx, qreal sy, const QRectF &lim, bool &sliceEnd, QPainter *painter)
{
    // https://gist.github.com/ChickenProp/3194723

    qreal dx = x2 - x1;
    qreal dy = y2 - y1;

    qreal limx = lim.x(), limy = lim.y();
    qreal v[4] = { -dx, dx, -dy, dy };
    qreal u[4] = { x1 - limx, lim.right() - x1, y1 - limy, lim.bottom() - y1 };
    qreal t[4];

    qreal tMax = Inf, tMin = -Inf;
    bool slice = true;
    for (int i=0; i<4; ++i) {
        if (v[i] != 0) {
            t[i] = u[i]/v[i];
            if (v[i] < 0 && tMin < t[i])
                tMin = t[i];
            if (v[i] > 0 && tMax > t[i])
                tMax = t[i];

        } else if (u[i] >= 0) {
            // Line is fully inside rect
            slice = false;
            break;
        } else {
            return; // Outside rect
        }
    }

    if (slice) {
        // Check if valid line
        if (tMin >= tMax || tMax < 0 || tMin > 1){
            return;
        }

        tMax = qMax(0.0, qMin(tMax, 1.0));
        tMin = qMax(0.0, qMin(tMin, 1.0));

        x2 = x1 + tMax*dx;
        x1 += tMin*dx;
        y2 = y1 + tMax*dy;
        y1 += tMin*dy;
    }

    // Transform into screen coords
    x1 = (x1 - limx)*sx + tx;
    x2 = (x2 - limx)*sx + tx;
    y1 = (y1 - limy)*sy + ty;
    y2 = (y2 - limy)*sy + ty;
    if (painter != 0) {
        painter->drawLine(x1, y1, x2, y2);
    } else if (path != 0) {
        if (sliceEnd)
            path->moveTo(x1, y1);
        path->lineTo(x2, y2);
    }
    sliceEnd = tMax < 1;
}

//!
//! \method precision
//! Return the base-10 precision of the float value.
//! E.g. 100 -> 3, 999-> 3, 3 -> 0, 0.1 -> -1, 0.00021 -> -4
//!
int Util::precision(qreal value)
{
    return qFloor(log10(qAbs(value)));
}

int Util::sign(qreal val)
{
    return (0 < val) - (val < 0);
}

int Util::sign(int val)
{
    return (0 < val) - (val < 0);
}

qreal Util::clamp(qreal d, qreal min, qreal max) {
    const qreal t = d < min ? min : d;
    return t > max ? max : t;
}

QString Util::formatReal(qreal value, int precision)
{
    // TODO: Monitor the performance of this...
    static QRegExp regZero("0+$");
    static QRegExp regDot("\\.$");
    QString result = QString::number(value, 'f', precision);
    if (precision > 0) {
        result.remove(regZero); // Remove any number of trailing 0's
        result.remove(regDot); // If the last character is just a '.' then remove it
    }
    return result;
}

QTransform Util::plotToView(QSizeF viewSize, QRectF lim, bool invertX, bool invertY)
{
    // Transform the plot coords to view coords
    qreal scaleX = viewSize.width()/(lim.width());
    qreal scaleY = viewSize.height()/(lim.height());
    qreal tx = 0;
    qreal ty = 0;
    if (invertX) {
        scaleX *= -1;
        tx = viewSize.width();
    }
    if (!invertY) {
        scaleY *= -1;
        ty = viewSize.height();
    }

    QTransform tran;
    tran.translate(tx, ty).scale(scaleX, scaleY).translate(-lim.x(), -lim.y());
    return tran;
}

void Util::plotToView(
        qreal viewWidth, qreal viewHeight, const QRectF &lim, bool invertX, bool invertY,
        qreal &tx, qreal &ty, qreal &sx, qreal &sy)
{
    sx = viewWidth/lim.width();
    sy = viewHeight/lim.height();
    if (invertX) {
        sx *= -1;
        tx = viewWidth;
    } else {
        tx = 0;
    }
    if (!invertY) {
        sy *= -1;
        ty = viewHeight;
    } else {
        ty = 0;
    }
}

QList<int>* Util::medianArray(int N, int s, QList<int>* lst)
{
    if (!lst) {
        lst = new QList<int>();
        lst->reserve(N);
    }

    if (N - s <= 2) {
        lst->append(s);
        if (N - s == 2)
            lst->append(s + 1);
        return lst;
    }

    int i = (s + N - 1)/2;
    lst->append(i);

    medianArray(i, s, lst);
    medianArray(N, i + 1, lst);

    return lst;
}

