#ifndef UTIL_H
#define UTIL_H

#include "../nutmeglibdecl.h"
#include <QtGui/QTransform>
#include <QList>
#include <QRectF>
#include <QPolygonF>
#include <QLineF>

#include <limits>
#define Inf std::numeric_limits<double>::infinity()
#define Nan std::numeric_limits<double>::signaling_NaN();

class Util {

public:
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

    static qreal clamp(qreal d, qreal min, qreal max);

    static QString formatReal(qreal value, int precision);

    static QTransform plotToView(QSizeF viewSize, QRectF dataLimits, bool invertX=false, bool invertY=true);

    static void plotToView(qreal viewWidth, qreal viewHeight, const QRectF &lim, bool invertX, bool invertY, qreal &tx, qreal &ty, qreal &sx, qreal &sy);
    static void drawLineSlice(QPainterPath *path, qreal x1, qreal y1, qreal x2, qreal y2, qreal tx, qreal ty, qreal sx, qreal sy, const QRectF &lim, bool &sliceEnd);
};

#endif // UTIL_H
