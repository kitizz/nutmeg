#include "util.h"
#include <QtCore/qmath.h>
#include <cmath>

Util::Util()
{
}

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

/*!
 * \fn Util::resizeRelativeRatio
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
 * \fn Util::cullLine
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

int Util::precision(qreal value)
{
    return qFloor(log10(value));
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
