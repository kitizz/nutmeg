#ifndef IMAGEPLOTBASE_H
#define IMAGEPLOTBASE_H

#include "plot2dbase.h"
#include <QPixmap>

class NUTMEGLIB_EXPORT ImagePlot : public Plot2DBase
{
    Q_OBJECT
    Q_PROPERTY(QVariantList binary READ binary WRITE setBinary NOTIFY binaryChanged)
    Q_PROPERTY(qreal xScale READ xScale WRITE setXScale NOTIFY xScaleChanged)
    Q_PROPERTY(qreal yScale READ yScale WRITE setYScale NOTIFY yScaleChanged)
    Q_PROPERTY(qreal xOffset READ xOffset WRITE setXOffset NOTIFY xOffsetChanged)
    Q_PROPERTY(qreal yOffset READ yOffset WRITE setYOffset NOTIFY yOffsetChanged)
//    Q_PROPERTY( name READ name WRITE setName NOTIFY nameChanged)

public:
    explicit ImagePlot(QQuickItem *parent = 0);

    QPixmap pixmap() const;

    QVariantList binary() const;
    void setBinary(const QVariantList &data);

    qreal xScale() const;
    void setXScale(qreal xScale);
    qreal yScale() const;
    void setYScale(qreal yScale);
    qreal xOffset() const;
    void setXOffset(qreal xOffset);
    qreal yOffset() const;
    void setYOffset(qreal yOffset);

signals:
    void binaryChanged(const QVariantList &arg); // Change to (const &) ?
    void xScaleChanged(qreal xScale);
    void yScaleChanged(qreal yScale);
    void xOffsetChanged(qreal xOffset);
    void yOffsetChanged(qreal yOffset);

public slots:

protected slots:
    virtual void updateDataLimits();

private:
    void updateImage();
    QVariantList m_data;
    QList<int> m_shape;
    QImage m_im;
    QPixmap m_pixmap;
    QByteArray m_binary;

    qreal m_xScale;
    qreal m_yScale;
    qreal m_xOffset;
    qreal m_yOffset;
};

#endif // IMAGEPLOTBASE_H
