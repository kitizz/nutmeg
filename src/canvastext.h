#ifndef CANVASTEXT_H
#define CANVASTEXT_H

#include "canvasshape.h"

class CanvasText : public CanvasShape
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

    Q_PROPERTY(QSizeF textScreenSize READ textScreenSize NOTIFY textScreenSizeChanged)

public:
    explicit CanvasText(QQuickItem *parent = 0);

    virtual void paint(QPainter *painter, const QTransform &tran, const QRectF &limits);

    QString text() const;
    void setText(QString arg);

    QFont font() const;
    void setFont(QFont arg);

    QColor color() const;
    void setColor(QColor arg);

    QSizeF textScreenSize() const;

signals:
    void textChanged(QString arg);
    void fontChanged(QFont arg);
    void colorChanged(QColor arg);

    void textScreenSizeChanged(QSizeF arg);

public slots:
    void updateTextSize();
//    void updateDataLimits();

private:
    QString m_text;
    QFont m_font;
    QFontMetricsF m_fontMetrics;
    QColor m_color;

    QSizeF m_textScreenSize;
    qreal m_textWidth;
    qreal m_textHeight;
};

#endif // CANVASTEXT_H
