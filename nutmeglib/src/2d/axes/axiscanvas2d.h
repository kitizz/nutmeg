#ifndef AXISCANVAS2D_H
#define AXISCANVAS2D_H

#include <QQuickPaintedItem>
#include <QStaticText>

#include "axis2dbase.h"

class NUTMEGLIB_EXPORT AxisCanvas2D : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(Axis2DBase* axis READ axis WRITE setAxis NOTIFY axisChanged)
    Q_PROPERTY(qreal scaling READ scaling WRITE setScaling NOTIFY scalingChanged)

    Q_PROPERTY(qreal titleMargin READ titleMargin WRITE setTitleMargin NOTIFY titleMarginChanged)
    Q_PROPERTY(qreal xLabelMargin READ xLabelMargin WRITE setXLabelMargin NOTIFY xLabelMarginChanged)
    Q_PROPERTY(qreal yLabelMargin READ yLabelMargin WRITE setYLabelMargin NOTIFY yLabelMarginChanged)
    Q_PROPERTY(qreal xTicksMargin READ xTicksMargin WRITE setXTicksMargin NOTIFY xTicksMarginChanged)
    Q_PROPERTY(qreal yTicksMargin READ yTicksMargin WRITE setYTicksMargin NOTIFY yTicksMarginChanged)

public:
    explicit AxisCanvas2D(QQuickItem *parent = 0);

    void paint(QPainter *painter);
    void updatePolish();

    Axis2DBase* axis() const;
    void setAxis(Axis2DBase* arg);

    qreal scaling() const;
    void setScaling(qreal arg);

    qreal titleMargin() const;
    void setTitleMargin(qreal arg);

    qreal xLabelMargin() const;
    void setXLabelMargin(qreal arg);

    qreal yLabelMargin() const;
    void setYLabelMargin(qreal arg);

    qreal xTicksMargin() const;
    void setXTicksMargin(qreal arg);

    qreal yTicksMargin() const;
    void setYTicksMargin(qreal arg);

signals:
    void axisChanged(Axis2DBase* arg);
    void scalingChanged(qreal arg);

    void titleMarginChanged(qreal arg);
    void xLabelMarginChanged(qreal arg);
    void yLabelMarginChanged(qreal arg);
    void xTicksMarginChanged(qreal arg);
    void yTicksMarginChanged(qreal arg);

public slots:
    void triggerUpdate();
    void updatePlotRect();

private:
    QList<qreal> plot2canvasX(QList<qreal> x);
    QList<qreal> plot2canvasY(QList<qreal> y);

    void prepareTexts();
    void prepareScaleAndOffset(AxisSpec *spec, int scaleExp, qreal offset, QStaticText *st);
    QSizeF prepareTickLabels(AxisSpec *spec, QHash<QString, QStaticText *> &labelTexts,
                            QList<QString> &tickStrings, int &scaleExp, qreal &offset);

    void drawTitle(QPainter *painter, const QRectF &plotRect);
    void drawXLabel(QPainter *painter, const QRectF &plotRect);
    void drawYLabel(QPainter *painter, const QRectF &plotRect);
    QStaticText *getStaticText(const QString &text);
    void drawXTickLabels(QList<qreal> xFrames, QPainter *painter, const QRectF &plotRect);
    void drawYTickLabels(QList<qreal> yFrames, QPainter *painter, const QRectF &plotRect);
    void drawXTicks(QList<qreal> major, QPainter *painter, const QRectF &plotRect);
    void drawYTicks(QList<qreal> major, QPainter *painter, const QRectF &plotRect);
    void drawScale(QPainter* painter, const QRectF &plotRect);
//    void drawYScale(QPainter* painter, const QRectF &plotRect);
    void drawGrid(QList<qreal> xMajor, QList<qreal> yMajor, QPainter *painter, const QRectF &plotRect);

    QFont getFont(const QFont &font);

    Axis2DBase* m_axis;

    qreal m_scaling;
    qreal m_textscaling;
    qreal m_titleMargin;
    qreal m_xLabelMargin;
    qreal m_yLabelMargin;
    qreal m_xTicksMargin;
    qreal m_yTicksMargin;

//    qreal m_titleHeight;
//    qreal m_xLabelHeight;
//    qreal m_yLabelWidth;
    qreal m_xTicksHeight;
    qreal m_yTicksWidth;

    QHash<QString, QStaticText*> m_xTickText;
    QHash<QString, QStaticText*> m_yTickText;
    QList<QStaticText*> m_tickTextPool; // Holds unused QStaticText objects for later
    QStaticText m_title;
    QStaticText m_xLabel, m_yLabel;
    int m_xScaleExp, m_yScaleExp;
    QStaticText m_xScaleText, m_yScaleText;
    qreal m_xOffset, m_yOffset;
//    QStaticText m_xOffsetText, m_yOffsetText;

    QList<QString> m_xTicks;
    QList<QString> m_yTicks;

};

#endif // AXISCANVAS2D_H
