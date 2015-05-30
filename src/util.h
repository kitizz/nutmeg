#ifndef UTIL_H
#define UTIL_H

#include <QList>
#include <QRectF>
#include <QPolygonF>
#include <QLineF>

#include <limits>
#define Inf std::numeric_limits<double>::infinity()

class Util
{
public:
    Util();

    /*!
     * \enum The AnchorSide enum
     * AnchorFit: Anchor to the side with the largest ratio so that whole
     * rect fits in the designated size.
     * AnchorWidth: Anchor the width
     * AnchorHeight: Anchor the height
     */
    enum AnchorSide { AnchorFit=0, AnchorWidth=1, AnchorHeight=2 };

    static QList<int>* medianArray(int N, int s=0, QList<int> *lst=0);
    static int randInt(int low, int high);
    static QList<int> *randomArray(int N);
    static qreal median(QList<qreal> lst);
    static int mode(QList<int> lst);
    static int min(QList<int> lst);
    static int max(QList<int> lst);
    static QList<qreal> diff(QList<qreal> lst);

    static qreal roundTo(qreal value, int precision);

    static void resizeRelativeRatio(QRectF *rect, QSizeF size, qreal aspectRatio,
                                    AnchorSide anchor=AnchorFit, QPointF center=QPoint());

    static QList<QPolygonF> cullLine(QPolygonF poly, QRectF rect);
    static QLineF rectSlice(QPointF p1, QPointF p2, QRectF r);

    static int precision(qreal value);
    static int sign(qreal value);
    static int sign(int value);

    static QString formatReal(qreal value, int precision);

};

#endif // UTIL_H
