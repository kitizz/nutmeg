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
    , m_xScaleExp(0), m_yScaleExp(0)
    , m_xOffset(0), m_yOffset(0)
    , m_xScaleText(QStaticText())
    , m_yScaleText(QStaticText())
//    , m_xOffsetText(QStaticText())
//    , m_yOffsetText(QStaticText())
    , m_xTicks(QList<QString>())
    , m_yTicks(QList<QString>())
{
    setRenderTarget(QQuickPaintedItem::FramebufferObject);
//    connect(this, &AxisCanvas2D::plotRectChanged, this, &AxisCanvas2D::update);
}

void AxisCanvas2D::paint(QPainter *painter)
{
    QRectF lim = m_axis->limits();
    QRectF plotRect = m_axis->updatePlotRect();

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

    drawTitle(painter, plotRect);
    drawXLabel(painter, plotRect);
    drawYLabel(painter, plotRect);

    drawXTickLabels(xMajor, painter, plotRect);
    drawYTickLabels(yMajor, painter, plotRect);

    drawScale(painter, plotRect);

    drawGrid(xMajor, yMajor, painter, plotRect);
    drawXTicks(xMajor, painter, plotRect);
    drawYTicks(yMajor, painter, plotRect);
}

void AxisCanvas2D::updatePolish()
{
    prepareTexts();
    updatePlotRect();
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
        connect(m_axis, &QQuickItem::heightChanged, this, &AxisCanvas2D::triggerUpdate);
        connect(m_axis, &QQuickItem::widthChanged, this, &AxisCanvas2D::triggerUpdate);
        triggerUpdate();
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
    polish();
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

    m_axis->setPreferredPlotRect(newRect);
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

void AxisCanvas2D::prepareTexts()
{
    m_title.setText(m_axis->title());
    m_xLabel.setText(m_axis->xAxis()->label());
    m_yLabel.setText(m_axis->yAxis()->label());

    AxisSpec *xSpec = m_axis->xAxis();
    AxisSpec *ySpec = m_axis->yAxis();

    m_xTicksHeight = prepareTickLabels(xSpec, m_xTickText, m_xTicks, m_xScaleExp, m_xOffset).height();
    m_yTicksWidth = prepareTickLabels(ySpec, m_yTickText, m_yTicks, m_yScaleExp, m_yOffset).width();

    prepareScaleAndOffset(xSpec, m_xScaleExp, m_xOffset, &m_xScaleText);
    prepareScaleAndOffset(ySpec, m_yScaleExp, m_yOffset, &m_yScaleText);
}

void AxisCanvas2D::prepareScaleAndOffset(AxisSpec *spec, int scaleExp, qreal offset, QStaticText *st)
{
    QFont font = spec->tickFont();
    font.setPointSize( font.pointSize()*m_scaling );

    static QString baseScale("\u00D71e%1");
    static QString baseOffset("%1 %2");
    static QString baseScaleOffset("\u00D71e%1 %2 %3");

    if (scaleExp != 0 || offset != 0) {
        QString scale = QString::number(scaleExp),
                sign = offset > 0 ? "+" : "-",
                off = QString::number(qAbs(offset));

        QString scaleText;
        if (scaleExp != 0 && offset != 0)
            scaleText = baseScaleOffset.arg(scale, sign, off);
        else if (scaleExp != 0)
            scaleText = baseScale.arg(scale);
        else
            scaleText = baseOffset.arg(sign, off);

        st->setText(scaleText);
    } else {
        st->setText("");
    }
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
                                      QList<QString> &tickStrings, int &scaleExp, qreal &offset)
{
    int prec = spec->tickPrecision();

    // We use QStaticText to precalculate the largest width before
    // actually drawing.
    QHash<QString, QStaticText*> newTicks;
    tickStrings.clear();
    QSizeF maxSize;

    // Find the maximum precision required
    // Assume: The precisions are all of the same sign, and
    //         "Zero" values don't have precision errors (e.g. 1e-17)

    scaleExp = 0;
    offset = 0;
    auto ticks = spec->majorTicksReal();
    if (ticks.length() == 0)
        return QSizeF();

    auto diff = Util::diff(ticks);
    QList<int> precisions;

    qreal pixelsPerUnit = spec->pixelSize()/(spec->max() - spec->min());
    foreach (qreal d, diff) {
        // Sometimes values that are meant to be 0.01 can be 0.00999...
        // due to precision errors. The "precision" of this then becomes
        // -3 when it should really be -2. This fixes that. It's a horrible
        // hack, but I can't be a really bad reason not to do this.
        if (d*pixelsPerUnit > 1)
            precisions << Util::precision(d*1.01);
    }
    int scalePrec = Util::min(precisions);

    // 3 cases: No scale required. Scale down from large numbers. Scale up from small numbers.
    scaleExp = 0;
    qreal scale = 1;
    offset = Util::roundTo(ticks[0], scalePrec + prec);
    if (scalePrec > prec || scalePrec - 1 < -prec) {
        // Numbers need to be scaled down || scaled up
        scaleExp = scalePrec;
        scale = qPow(10, -scaleExp);

    } else if (scalePrec > 0) {
        // Keep the *total* precision set to prec
        // if there are digits to the left of the decimal
        prec -= scalePrec;
    }

    foreach (qreal v, ticks) {
        QString text = Util::formatReal((v - offset)*scale, prec - 1);
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

void AxisCanvas2D::drawTitle(QPainter *painter, const QRectF &plotRect)
{
    QFont font = m_axis->titleFont();
    font.setPointSize( font.pointSize()*m_scaling );
    painter->setFont(font);

    qreal x = 0.5*(width() - m_title.size().width());
    qreal y = m_scaling*m_axis->margin()->top();
    painter->drawStaticText(x, y, m_title);
}

void AxisCanvas2D::drawXLabel(QPainter *painter, const QRectF &plotRect)
{
    QFont font = m_axis->xAxis()->labelFont();
    font.setPointSize( font.pointSize()*m_scaling );
    painter->setFont(font);

    qreal x = m_scaling*plotRect.center().x() - 0.5*m_xLabel.size().width();
    qreal y = plotRect.bottom() + m_xTicksMargin + m_xTicksHeight/m_scaling + m_xLabelMargin;
    painter->drawStaticText(x, m_scaling*y, m_xLabel);
}

void AxisCanvas2D::drawYLabel(QPainter *painter, const QRectF &plotRect)
{
    QFont font = m_axis->yAxis()->labelFont();
    font.setPointSize( font.pointSize()*m_scaling );
    painter->setFont(font);

    qreal x = m_scaling*m_axis->margin()->left();
    qreal y = m_scaling*plotRect.center().y();

    painter->save();
    painter->translate(x, y);
    painter->rotate(-90);

    painter->drawStaticText(-0.5*m_yLabel.size().width(), 0, m_yLabel);

    painter->restore();
}

void AxisCanvas2D::drawXTickLabels(QList<qreal> xFrames, QPainter *painter, const QRectF &plotRect)
{
    QFont font = m_axis->yAxis()->tickFont();
    font.setPointSize( font.pointSize()*m_scaling );
    painter->setFont(font);

    qreal y = plotRect.bottom();

    for (int i=0; i<xFrames.size(); ++i) {
        qreal x = xFrames[i];
        QString text = m_xTicks[i];
        QStaticText *st = m_xTickText[text];
        QSizeF sz = st->size();

        painter->drawStaticText(m_scaling*x - sz.width()/2, m_scaling*(y + m_xTicksMargin), *st);
    }
}


void AxisCanvas2D::drawYTickLabels(QList<qreal> yFrames, QPainter *painter, const QRectF &plotRect)
{
    QFont font = m_axis->yAxis()->tickFont();
    font.setPointSize( font.pointSize()*m_scaling );
    painter->setFont(font);

    qreal x = plotRect.x();

    for (int i=0; i<yFrames.size(); ++i) {
        qreal y = yFrames[i];
        QString text = m_yTicks[i];
        QStaticText *st = m_yTickText[text];
        QSizeF sz = st->size();

        painter->drawStaticText((x - m_yTicksMargin)*m_scaling - sz.width(), m_scaling*y - 0.5*sz.height(), *st);
    }
}

void AxisCanvas2D::drawXTicks(QList<qreal> major, QPainter *painter, const QRectF &plotRect)
{
    QVector<QPointF> lines;

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

void AxisCanvas2D::drawYTicks(QList<qreal> major, QPainter *painter, const QRectF &plotRect)
{
//    QRectF lim = m_axis->limits();
    QVector<QPointF> lines;

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

void AxisCanvas2D::drawScale(QPainter *painter, const QRectF &plotRect)
{
    QFont font = m_axis->yAxis()->tickFont();
    font.setPointSize( font.pointSize()*m_scaling );
    painter->setFont(font);

    qreal xScale_x = m_scaling*plotRect.right() - m_xScaleText.size().width();
    qreal xScale_y = plotRect.bottom() + m_xTicksMargin + m_xTicksHeight/m_scaling + m_xLabelMargin;
    painter->drawStaticText(xScale_x, m_scaling*xScale_y, m_xScaleText);

    qreal yScale_x = m_scaling*plotRect.left();
    qreal yScale_y = m_scaling*plotRect.top() - m_yScaleText.size().height() - 2;
    painter->drawStaticText(yScale_x, yScale_y, m_yScaleText);
}

void AxisCanvas2D::drawGrid(QList<qreal> xMajor, QList<qreal> yMajor, QPainter *painter, const QRectF &plotRect)
{
    QVector<QPointF> lines;

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
