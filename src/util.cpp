#include "util.h"
#include <QtCore/qmath.h>

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
