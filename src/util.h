#ifndef UTIL_H
#define UTIL_H

#include <QList>
#include <QRectF>

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

    // TODO: The following functions should find their way to a static util library...
    static QList<int>* medianArray(int N, int s=0, QList<int> *lst=0);
    static int randInt(int low, int high);
    static QList<int> *randomArray(int N);

    static void resizeRelativeRatio(QRectF *rect, QSizeF size, qreal aspectRatio,
                                    AnchorSide anchor=AnchorFit, QPointF center=QPoint());
};

#endif // UTIL_H
