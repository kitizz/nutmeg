#include "axiscanvas2d.h"
#include <QGuiApplication>
#include <QScreen>

#include <QPen>
#include <QStaticText>

#include "util.h"
#include <QtMath>
#include <cmath>

AxisCanvas2D::AxisCanvas2D(QQuickItem *parent)
    : QQuickPaintedItem(parent)
    , m_axis(0)
    , m_scaling(QGuiApplication::primaryScreen()->devicePixelRatio())
    , m_titleMargin(5)
    , m_xLabelMargin(5)
    , m_yLabelMargin(5)
    , m_xTicksMargin(5)
    , m_yTicksMargin(5)
    , m_xTicksHeight(0)
    , m_yTicksWidth(0)
    , m_xTickText(QHash<QString, QStaticText*>())
    , m_yTickText(QHash<QString, QStaticText*>())
    , m_tickTextPool(QList<QStaticText*>())
    , m_title(QStaticText())
    , m_xLabel(QStaticText())
    , m_yLabel(QStaticText())
    , m_xScale(QStaticText())
    , m_yScale(QStaticText())
    , m_xOffset(QStaticText())
    , m_yOffset(QStaticText())
    , m_xTicks(QList<QString>())
    , m_yTicks(QList<QString>())
{
    setRenderTarget(QQuickPaintedItem::FramebufferObject);
//    connect(this, &AxisCanvas2D::plotRectChanged, this, &AxisCanvas2D::update);
}

void AxisCanvas2D::paint(QPainter *painter)
{
    QRectF lim = m_axis->limits();
    QRectF plotRect = m_axis->plotRect();

    QList<qreal> xMajor, yMajor;

    foreach (const qreal tick, m_axis->xAxis()->majorTicksReal()) {
        qreal xNorm = (tick - lim.left()) / lim.width();
        if (m_axis->xAxis()->inverted())
            xNorm = 1 - xNorm;
        xMajor << plotRect.left() + plotRect.width()*xNorm;
    }

    foreach (const qreal tick, m_axis->yAxis()->majorTicksReal()) {
        qreal yNorm = (tick - lim.top()) / lim.height();
        if (m_axis->yAxis()->inverted())
            yNorm = 1 - yNorm;
        yMajor << plotRect.bottom() - plotRect.height()*yNorm;
    }

    prepareTexts();
    updatePlotRect();

    drawTitle(painter);
    drawXLabel(painter);
    drawYLabel(painter);

    drawXTickLabels(xMajor, painter);
    drawYTickLabels(yMajor, painter);

    drawGrid(xMajor, yMajor, painter);
    drawXTicks(xMajor, painter);
    drawYTicks(yMajor, painter);
}



Axis2DBase *AxisCanvas2D::axis() const
{
    return m_axis;
}

void AxisCanvas2D::setAxis(Axis2DBase *arg)
{
    if (m_axis == arg) return;

    if (m_axis) {
        m_axis->setCanvas(0);
        m_axis->disconnect(this);
        m_axis->xAxis()->disconnect(this);
        m_axis->yAxis()->disconnect(this);
    }

    m_axis = arg;

    if (m_axis) {
        m_axis->setCanvas(this);
        connect(m_axis, &Axis2DBase::limitsChanged, this, &AxisCanvas2D::triggerUpdate);
        connect(m_axis, &Axis2DBase::plotRectChanged, this, &AxisCanvas2D::triggerUpdate);
        connect(m_axis->xAxis(), &AxisSpec::majorTicksChanged, this, &AxisCanvas2D::triggerUpdate);
        connect(m_axis->xAxis(), &AxisSpec::minorTicksChanged, this, &AxisCanvas2D::triggerUpdate);
        connect(m_axis->yAxis(), &AxisSpec::majorTicksChanged, this, &AxisCanvas2D::triggerUpdate);
        connect(m_axis->yAxis(), &AxisSpec::minorTicksChanged, this, &AxisCanvas2D::triggerUpdate);
    }
    emit axisChanged(arg);
}

qreal AxisCanvas2D::scaling() const
{
    return m_scaling;
}

void AxisCanvas2D::setScaling(qreal arg)
{
    if (m_scaling == arg) return;
    m_scaling = arg;
    emit scalingChanged(arg);
}

void AxisCanvas2D::triggerUpdate()
{
    update();
}

void AxisCanvas2D::updatePlotRect()
{
    AxisMargins *mrg = m_axis->margin();
    QRect newRect;
    newRect.setLeft(mrg->left() + m_yLabelMargin + m_yTicksMargin
                    + (m_yTicksWidth + m_yLabel.size().height())/m_scaling);
    newRect.setTop(mrg->top() + m_titleMargin + m_title.size().height()/m_scaling);
    newRect.setRight(m_axis->width() - mrg->right());
    newRect.setBottom(m_axis->height() - mrg->bottom() - m_xTicksMargin - m_xLabelMargin
                      - (m_xTicksHeight + m_xLabel.size().height())/m_scaling);

    m_axis->setPlotRect(newRect);
}

qreal AxisCanvas2D::titleMargin() const
{
    return m_titleMargin;
}

void AxisCanvas2D::setTitleMargin(qreal arg)
{
    if (m_titleMargin == arg)
        return;

    m_titleMargin = arg;
    emit titleMarginChanged(arg);
}

qreal AxisCanvas2D::xLabelMargin() const
{
    return m_xLabelMargin;
}

void AxisCanvas2D::setXLabelMargin(qreal arg)
{
    if (m_xLabelMargin == arg)
        return;

    m_xLabelMargin = arg;
    emit xLabelMarginChanged(arg);
}

qreal AxisCanvas2D::yLabelMargin() const
{
    return m_yLabelMargin;
}

void AxisCanvas2D::setYLabelMargin(qreal arg)
{
    if (m_yLabelMargin == arg)
        return;

    m_yLabelMargin = arg;
    emit yLabelMarginChanged(arg);
}

qreal AxisCanvas2D::xTicksMargin() const
{
    return m_xTicksMargin;
}

void AxisCanvas2D::setXTicksMargin(qreal arg)
{
    if (m_xTicksMargin == arg)
        return;

    m_xTicksMargin = arg;
    emit xTicksMarginChanged(arg);
}

qreal AxisCanvas2D::yTicksMargin() const
{
    return m_yTicksMargin;
}

void AxisCanvas2D::setYTicksMargin(qreal arg)
{
    if (m_yTicksMargin == arg)
        return;

    m_yTicksMargin = arg;
    emit yTicksMarginChanged(arg);
}

QList<qreal> AxisCanvas2D::plot2canvasX(QList<qreal> xValues)
{
    QRectF lim = m_axis->limits();
    QRectF plotRect = m_axis->plotRect();

    QList<qreal> newX;
    foreach (qreal x, xValues) {
        qreal xNorm = (x - lim.left()) / lim.width();
        if (m_axis->xAxis()->inverted())
            xNorm = 1 - xNorm;
        newX << plotRect.left() + plotRect.width()*xNorm;
    }

    return newX;
}

QList<qreal> AxisCanvas2D::plot2canvasY(QList<qreal> yValues)
{
    QRectF lim = m_axis->limits();
    QRectF plotRect = m_axis->plotRect();

    QList<qreal> newY;
    foreach (qreal y, yValues) {
        qreal yNorm = (y - lim.top()) / lim.height();
        if (m_axis->yAxis()->inverted())
            yNorm = 1 - yNorm;
        newY << plotRect.bottom() - plotRect.height()*yNorm;
    }

    return newY;
}

void AxisCanvas2D::drawXTicks(QList<qreal> major, QPainter *painter)
{
    QVector<QPointF> lines;
    QRectF plotRect = m_axis->plotRect();

    qreal majorSize = m_axis->xAxis()->majorTickSize();
//    qreal minorSize = m_axis->xAxis()->minorTickSize();

    // Calculate how to draw the tick directions along the x-axis
    AxisSpec::TickDirection xDir = m_axis->xAxis()->tickDir();
    qreal y1 = plotRect.bottom(), y2 = plotRect.bottom();
    if (xDir == AxisSpec::In || xDir == AxisSpec::InOut)
        y1 -= majorSize;
    if (xDir == AxisSpec::Out || xDir == AxisSpec::InOut)
        y2 += majorSize;
    else
        y2 -= 1;

    foreach (qreal x, major) {
        lines << m_scaling*QPointF(x, y1);
        lines << m_scaling*QPointF(x, y2);
    }

    LineSpec *spec = m_axis->xAxis()->majorLine();
    QPen pen;
    pen.setColor(spec->color());
    pen.setWidth(spec->width());
    painter->setPen(pen);
    painter->drawLines(lines);
}

void AxisCanvas2D::drawYTicks(QList<qreal> major, QPainter *painter)
{
//    QRectF lim = m_axis->limits();
    QVector<QPointF> lines;
    QRectF plotRect = m_axis->plotRect();

    qreal majorSize = m_axis->yAxis()->majorTickSize();
//    qreal minorSize = m_axis->yAxis()->minorTickSize();

    // Calculate how to draw the tick directions along the y-axis
    AxisSpec::TickDirection yDir = m_axis->yAxis()->tickDir();
    qreal x1 = plotRect.left(), x2 = plotRect.left();
    if (yDir == AxisSpec::In || yDir == AxisSpec::InOut)
        x1 += majorSize;
    if (yDir == AxisSpec::Out || yDir == AxisSpec::InOut)
        x2 -= majorSize;
    else
        x2 += 1;

    foreach (qreal y, major) {
        lines << m_scaling*QPointF(x1, y);
        lines << m_scaling*QPointF(x2, y);
    }

    LineSpec *spec = m_axis->yAxis()->majorLine();
    QPen pen;
    pen.setColor(spec->color());
    pen.setWidth(spec->width());
    painter->setPen(pen);
    painter->drawLines(lines);
}

void AxisCanvas2D::drawGrid(QList<qreal> xMajor, QList<qreal> yMajor, QPainter *painter)
{
    QVector<QPointF> lines;
    QRectF plotRect = m_axis->plotRect();

    if (m_axis->grid()->axes() & AxisGrid::X) {
        foreach (qreal x, xMajor) {
            lines << m_scaling*QPointF(x, plotRect.top());
            lines << m_scaling*QPointF(x, plotRect.bottom());
        }
    }
    if (m_axis->grid()->axes() & AxisGrid::Y) {
        foreach (qreal y, yMajor) {
            lines << m_scaling*QPointF(plotRect.left(), y);
            lines << m_scaling*QPointF(plotRect.right(), y);
        }
    }

    LineSpec *spec = m_axis->grid()->majorLine();
    QPen pen;
    pen.setColor(spec->color());
    pen.setWidth(spec->width());
    pen.setStyle(LineSpec::styleMap[spec->style()]);
    painter->setPen(pen);
    painter->drawLines(lines);
}

void AxisCanvas2D::prepareTexts()
{
    m_title.setText(m_axis->title());
    m_xLabel.setText(m_axis->xAxis()->label());
    m_yLabel.setText(m_axis->yAxis()->label());

    AxisSpec *xSpec = m_axis->xAxis();
    AxisSpec *ySpec = m_axis->yAxis();
    qreal xScale = prepareScale(xSpec, m_axis->maxX() - m_axis->minX(), &m_xScale);
    qreal yScale = prepareScale(ySpec, m_axis->maxY() - m_axis->minY(), &m_yScale);

    m_xTicksHeight = prepareTickLabels(xSpec, m_xTickText, m_xTicks, xScale).height();
    m_yTicksWidth = prepareTickLabels(ySpec, m_yTickText, m_yTicks, yScale).width();
}

qreal AxisCanvas2D::prepareScale(AxisSpec *spec, qreal range, QStaticText *st)
{
    int prec = spec->tickPrecision();
    QFont font = spec->labelFont();
    font.setPointSize( font.pointSize()*m_scaling );

    int rangePrec = Util::precision(range);

    qreal scale = 1;
    if (qAbs(rangePrec) > prec) {
        scale = pow(10, -rangePrec);
        QString scaleText;
        scaleText.sprintf("1e%d", -rangePrec);
        st->setText(scaleText);
    } else {
        st->setText("");
    }

    return scale;
}

QStaticText *AxisCanvas2D::getStaticText(const QString &text)
{
    if (m_tickTextPool.isEmpty()) {
        return new QStaticText(text);
    } else {
        QStaticText *st = m_tickTextPool.takeLast();
        st->setText(text);
        return st;
    }
}

QSizeF AxisCanvas2D::prepareTickLabels(AxisSpec *spec, QHash<QString, QStaticText*> &staticTexts,
                                      QList<QString> &tickStrings, qreal scale)
{
    int prec = spec->tickPrecision();

    // We use QStaticText to precalculate the largest width before
    // actually drawing.
    QHash<QString, QStaticText*> newTicks;
    tickStrings.clear();
    QSizeF maxSize;
    foreach (qreal v, spec->majorTicksReal()) {
        QString text;
        text.sprintf("%.*g", prec + 2, v*scale);
        tickStrings.append(text);
        QStaticText *st = staticTexts.value(text, 0);
        if (!st)
            st = getStaticText(text);
        else
            staticTexts.remove(text);
        newTicks.insert(text, st);
        // Precalculate the width of the text
        maxSize = maxSize.expandedTo(st->size());
    }

    // Move the unused Texts into the pool for later
    foreach (QStaticText* st, staticTexts.values())
        m_tickTextPool.append(st);
    staticTexts = newTicks;

    return maxSize;
}

void AxisCanvas2D::drawTitle(QPainter *painter)
{
    QFont font = m_axis->titleFont();
    font.setPointSize( font.pointSize()*m_scaling );
    painter->setFont(font);

    qreal x = 0.5*(width() - m_title.size().width());
    qreal y = m_scaling*m_axis->margin()->top();
    painter->drawStaticText(x, y, m_title);
}

void AxisCanvas2D::drawXLabel(QPainter *painter)
{
    QFont font = m_axis->xAxis()->labelFont();
    font.setPointSize( font.pointSize()*m_scaling );
    painter->setFont(font);

    QRectF plotRect = m_axis->plotRect();
    qreal x = m_scaling*plotRect.center().x() - 0.5*m_xLabel.size().width();
    qreal y = plotRect.bottom() + m_xTicksMargin + m_xTicksHeight/m_scaling + m_xLabelMargin;
    painter->drawStaticText(x, m_scaling*y, m_xLabel);
}

void AxisCanvas2D::drawYLabel(QPainter *painter)
{
    QFont font = m_axis->yAxis()->labelFont();
    font.setPointSize( font.pointSize()*m_scaling );
    painter->setFont(font);

    QRectF plotRect = m_axis->plotRect();
    qreal x = m_scaling*m_axis->margin()->left();
    qreal y = m_scaling*plotRect.center().y();

    painter->save();
    painter->translate(x, y);
    painter->rotate(-90);

    painter->drawStaticText(-0.5*m_yLabel.size().width(), 0, m_yLabel);

    painter->restore();
}

void AxisCanvas2D::drawXTickLabels(QList<qreal> xFrames, QPainter *painter)
{
    QFont font = m_axis->yAxis()->labelFont();
    font.setPointSize( font.pointSize()*m_scaling );
    painter->setFont(font);

    qreal y = m_axis->plotRect().bottom();

    for (int i=0; i<xFrames.size(); ++i) {
        qreal x = xFrames[i];
        QString text = m_xTicks[i];
        QStaticText *st = m_xTickText[text];
        QSizeF sz = st->size();

        painter->drawStaticText(m_scaling*x - sz.width()/2, m_scaling*(y + m_xTicksMargin), *st);
    }
}


void AxisCanvas2D::drawYTickLabels(QList<qreal> yFrames, QPainter *painter)
{
    QFont font = m_axis->yAxis()->labelFont();
    font.setPointSize( font.pointSize()*m_scaling );
    painter->setFont(font);

    qreal x = m_axis->plotRect().x();

    for (int i=0; i<yFrames.size(); ++i) {
        qreal y = yFrames[i];
        QString text = m_yTicks[i];
        QStaticText *st = m_yTickText[text];
        QSizeF sz = st->size();

        painter->drawStaticText((x - m_yTicksMargin)*m_scaling - sz.width(), m_scaling*y - 0.5*sz.height(), *st);
    }
}
