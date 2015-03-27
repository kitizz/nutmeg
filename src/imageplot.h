#ifndef IMAGEPLOTBASE_H
#define IMAGEPLOTBASE_H

#include "plot2dbase.h"
#include <QPixmap>

class ImagePlot : public Plot2DBase
{
    Q_OBJECT
    Q_PROPERTY(QByteArray binary READ binary WRITE setBinary NOTIFY binaryChanged)

public:
    explicit ImagePlot(QQuickItem *parent = 0);

    QPixmap pixmap() const;

    QByteArray binary() const;
    void setBinary(QByteArray arg);

signals:
    void binaryChanged(QByteArray arg); // Change to (const &) ?

public slots:

protected slots:
    virtual void updateDataLimits();

private:
    void updateImage();
    QPixmap m_pixmap;
    QByteArray m_binary;

};

#endif // IMAGEPLOTBASE_H
