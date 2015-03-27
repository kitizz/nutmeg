#include "canvastext.h"

CanvasText::CanvasText(QQuickItem *parent)
    : CanvasShape(parent)
    , m_text("")
    , m_font(QFont())
    , m_fontMetrics(QFontMetricsF(m_font))
    , m_color(Qt::black)
    , m_textScreenSize(QSizeF())
    , m_textWidth(0)
    , m_textHeight(0)
{
    QMap<QString, QString> props;
    props.insert("text", "text");
    props.insert("font", "font");
    props.insert("color", "color");
    props.insert("x", "x");
    props.insert("y", "y");
    registerProperties(props);

    connect(this, &CanvasText::textChanged, this, &CanvasText::updateTextSize);
    connect(this, &CanvasText::fontChanged, this, &CanvasText::updateTextSize);
}

void CanvasText::paint(QPainter *painter, const QTransform &tran, const QRectF &limits)
{
    qreal w = m_textScreenSize.width()/tran.m11();
    qreal h = m_textScreenSize.height()/tran.m22();
    qreal x = this->x();
    qreal y = this->y();

    // Let's not waste time here...
    if (!limits.intersects(QRectF(x, y, abs(w), abs(h)))) return;

    // This is done because of the way drawing text is decoupled from the axis limits
    if (w < 0) x -= w;
    if (h > 0) y += h;

    QPen pen;
    pen.setColor(m_color);

    painter->setPen(pen);
    painter->setFont(m_font);

    QPointF pos = tran.map(QPointF(x, y));
    painter->drawText(pos, m_text);
}

QString CanvasText::text() const
{
    return m_text;
}

void CanvasText::setText(QString arg)
{
    if (m_text == arg) return;
    m_text = arg;
    emit textChanged(arg);
}

QFont CanvasText::font() const
{
    return m_font;
}

void CanvasText::setFont(QFont arg)
{
    if (m_font == arg) return;
    m_font = arg;
    m_fontMetrics = QFontMetricsF(m_font);
    emit fontChanged(arg);
}

QColor CanvasText::color() const
{
    return m_color;
}

void CanvasText::setColor(QColor arg)
{
    if (m_color == arg) return;
    m_color = arg;
    fillSpec()->setColor(arg);
    emit colorChanged(arg);
}

/*!
 * \fn CanvasText::textBounds
 * \return The bounds of the text in screen coords. Note that the position
 * of the bounds is always zero. The width and height are the values of interest.
 */
QSizeF CanvasText::textScreenSize() const
{
    return m_textScreenSize;
}

void CanvasText::updateTextSize()
{
    QRectF bounds = m_fontMetrics.boundingRect(m_text);
    m_textScreenSize = QSizeF(bounds.width(), bounds.height());
    updateDataLimits();
    emit textScreenSizeChanged(m_textScreenSize);
}
