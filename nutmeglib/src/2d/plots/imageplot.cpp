#include "imageplot.h"
#include <QElapsedTimer>

ImagePlot::ImagePlot(QQuickItem *parent)
    : Plot2DBase(parent)
    , m_data(QVariantList())
    , m_shape(QList<int>())
    , m_im(QImage())
    , m_pixmap(QPixmap())
    , m_binary(QByteArray())
    , m_xScale(1)
    , m_yScale(1)
    , m_xOffset(0)
    , m_yOffset(0)
{
    QMap<QString,QString> props;
    props.insert("binary", "binary");
    props.insert("xScale", "xScale");
    props.insert("yScale", "yScale");
    props.insert("xOffset", "xOffset");
    props.insert("yOffset", "yOffset");
//    props.insert("raw", "raw");
    registerProperties(props);
}

QPixmap ImagePlot::pixmap() const
{
    return m_pixmap;
}

NDArray ImagePlot::binary() const
{
    return m_data;
}

void ImagePlot::setBinary(const NDArray &data)
{
    qDebug() << "Attempt to set binary...";
    qDebug() << "Shape:" << data.shape();

    m_data = data;
    updateImage();
    emit binaryChanged(m_data);
}

qreal ImagePlot::xScale() const
{
    return m_xScale;
}

void ImagePlot::setXScale(qreal xScale)
{
    if (m_xScale == xScale)
        return;

    m_xScale = xScale;
    emit xScaleChanged(xScale);
}

qreal ImagePlot::yScale() const
{
    return m_yScale;
}

void ImagePlot::setYScale(qreal yScale)
{
    if (m_yScale == yScale)
        return;

    m_yScale = yScale;
    emit yScaleChanged(yScale);
}

qreal ImagePlot::xOffset() const
{
    return m_xOffset;
}

void ImagePlot::setXOffset(qreal xOffet)
{
    if (m_xOffset == xOffet)
        return;

    m_xOffset = xOffet;
    emit xOffsetChanged(xOffet);
}

qreal ImagePlot::yOffset() const
{
    return m_yOffset;
}

void ImagePlot::setYOffset(qreal yOffset)
{
    if (m_yOffset == yOffset)
        return;

    m_yOffset = yOffset;
    emit yOffsetChanged(yOffset);
}

void ImagePlot::updateDataLimits()
{
//    qDebug() << "\tSetting imageplot limits" << m_pixmap.size();
    QSize sz = m_pixmap.size();
    qreal w = sz.width() * m_xScale;
    qreal h = sz.height() * m_yScale;
    qreal x = m_xOffset;
    qreal y = m_yOffset;
    qDebug() << "Image data lim:" << QRectF(x, y, w, h);
    setDataLimits(QRectF(x, y, w, h));
}

void ImagePlot::updateImage()
{
    // TODO: Consider using this perhaps for better speed in the future: https://code.google.com/p/lz4/
    // Also potentially useful if images are being sent over the network

    // Work out the size of the image
    auto shape = m_data.shape();
    qDebug() << "Shape:" << m_data.shape();
    int h, w, d=1;
    if (shape.length() < 2) {
        qWarning() << "Image binary requires at least 2 dimensions.";
        return; // No good
    }
    // Load em up
    h = shape[0];
    w = shape[1];
    if (shape.length() > 2) {
        d = shape[2];
    }

//    qDebug() << "\t\tImage size:" << h << w << d;

    if (m_im.width() != w || m_im.height() != h) {
        m_im = QImage(w, h, QImage::Format_RGB888);
    }

    QElapsedTimer tim; tim.start();
    if (d == 1) {
        // Must triplicate values because QImage doesn't appear to support grayscale
        uchar *src = (uchar*)m_data.data();
        for (int row = 0; row < h; ++row) {
            uchar *line = m_im.scanLine(row);
            uchar *end = line + 3*w;
            while (line < end) {
                *(line) = *src;
                *(line + 1) = *src;
                *(line + 2) = *src;
                src += 1;
                line += 3;
            }
        }

    } else if (d == 3) {
        // Use memcpy as the values come in
        uchar *src = (uchar*)m_data.data();
        for (int row = 0; row < h; ++row) {
            uchar *line = m_im.scanLine(row);
            memcpy(line, src, 3*w);
            src += 3*w;
        }
    }

    // This conversion takes almost no time - so probably no copying =)
    bool success = m_pixmap.convertFromImage(m_im);

    if (success) {
        updateDataLimits();
        canvas()->triggerUpdate();
    }
}
