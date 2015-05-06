#include "imageplot.h"

ImagePlot::ImagePlot(QQuickItem *parent)
    : Plot2DBase(parent)
    , m_pixmap(QPixmap())
    , m_binary(QByteArray())
{
    QMap<QString,QString> props;
    props.insert("binary", "binary");
    registerProperties(props);
}

QPixmap ImagePlot::pixmap() const
{
    return m_pixmap;
}

QByteArray ImagePlot::binary() const
{
    return m_binary;
}

void ImagePlot::setBinary(QByteArray arg)
{
    if (m_binary == arg) return;
    qDebug() << "\tBinary set" << arg.length();
    m_binary = arg;
    updateImage();
    emit binaryChanged(arg);
}

void ImagePlot::updateDataLimits()
{
    qDebug() << "\tSetting imageplot limits" << m_pixmap.size();
    setDataLimits(QRectF(QPoint(), m_pixmap.size()));
}

void ImagePlot::updateImage()
{
    bool success = m_pixmap.loadFromData(m_binary);

    if (success) updateDataLimits();
}
