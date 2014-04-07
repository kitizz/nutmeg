#include "lineplot.h"
#include <QColor>
#include <QtGui/QGuiApplication>
#include <QWindow>

QMap<QString, QColor> LineSpec::colorMap = LineSpec::generateColorMap();
QMap<QString, Qt::PenStyle> LineSpec::styleMap = LineSpec::generateStyleMap();

LinePlot::LinePlot(QQuickItem *parent)
    : PlotBase(parent)
    , m_settingData(false)
    , m_xData(QList<qreal>())
    , m_yData(QList<qreal>())
    , m_dataMin(QPointF())
    , m_dataMax(QPointF())
    , m_line(QList<QPointF>())
    , m_lineColor(QColor(Qt::blue))
    , m_lineStyle(QString("-"))
    , m_lineSpec(LineSpec("-b"))
    , m_lineWidth(1)
    , m_markerColor(QColor(Qt::black))
    , m_markerStyle(QString())
{
    QMap<QString,QString> props;
    props.insert("x", "xData");
    props.insert("y", "yData");
    props.insert("lineColor", "lineColor");
    props.insert("lineStyle", "lineStyle");
    props.insert("lineSpec", "lineSpec");
    props.insert("markerColor", "markerColor");
    props.insert("markerStyle", "markerStyle");
    registerProperties(props);

    connect(this, SIGNAL(yDataChanged(QList<qreal>)), SLOT(updateData()));
    connect(this, SIGNAL(xDataChanged(QList<qreal>)), SLOT(updateData()));
}

QList<qreal> LinePlot::xData() const
{
    return m_xData;
}

void LinePlot::setXData(QList<qreal> arg)
{
    if (m_xData == arg) return;
    m_xData = arg;
    qDebug() << "\tsetXData:" << m_xData.length();
    emit xDataChanged(arg);
}

QList<qreal> LinePlot::yData() const
{
    return m_yData;
}

void LinePlot::setYData(QList<qreal> arg)
{
    if (m_yData == arg) return;
    m_yData = arg;
    emit yDataChanged(arg);
}

QRectF LinePlot::dataLimits() const
{
    return m_dataLimits;
}

QColor LinePlot::lineColor() const
{
    return m_lineColor;
}

void LinePlot::setLineColor(QColor arg)
{
    if (m_lineColor == arg) return;
    m_lineColor = arg;
    // Clear the color description from the linespec, this separates the linespec from the color
    m_lineSpec.color = "";
    emit lineColorChanged(arg);
    emit lineSpecChanged(m_lineSpec.toString());
}

QString LinePlot::lineStyle() const
{
    return m_lineStyle;
}

/*!
 * @brief LinePlot::setLineStyle
 * @param style Accepted linestyles are ("-", "--", ":", "-.")
 * \table
 * \header
 *  \o LineStyle
 *  \o Appearance
 * \row
 *  \o "-"
 *  \o A solid line
 * \row
 *  \o "--"
 *  \o A dashed line
 * \row
 *  \o ":"
 *  \o A dotted line
 * \row
 *  \o "-."
 *  \o A dot-dashed line
 *
 */
void LinePlot::setLineStyle(const QString &style)
{
    if (m_lineStyle == style) return;
    if (!LineSpec::styleMap.contains(style)) {
        qWarning() << "Provided style, \"" + style + "\", not available.";
        return;
    }
    m_lineStyle = style;
    m_lineSpec.style = style;
    emit lineStyleChanged(style);
    emit lineSpecChanged(m_lineSpec.toString());
}

QString LinePlot::lineSpec() const
{
    return m_lineSpec.toString();
}

void LinePlot::setLineSpec(const QString &arg)
{
    if (m_lineSpec.toString() == arg) return;

    LineSpec spec(arg);
    if (!spec.isValid()) {
        qWarning() << Q_FUNC_INFO << "Invalid lineSpec," << arg;
        return;
    }
    m_lineSpec = spec;
    emit lineSpecChanged(arg);

    if (!spec.color.isEmpty()) {
        m_lineColor = LineSpec::colorMap[spec.color];
        emit lineColorChanged(m_lineColor);
        m_lineStyle = spec.style;
        emit lineStyleChanged(m_lineStyle);
    }
}

QColor LinePlot::markerColor() const
{
    return m_markerColor;
}

void LinePlot::setMarkerColor(QColor arg)
{
    if (m_markerColor == arg) return;
    m_markerColor = arg;
    emit markerColorChanged(arg);
}

QString LinePlot::markerStyle() const
{
    return m_markerStyle;
}

void LinePlot::setMarkerStyle(const QString &arg)
{
    if (m_markerStyle == arg) return;
    if (!arg.isEmpty() && !LineSpec::markers().contains(arg)) {
        qWarning() << Q_FUNC_INFO << "Invalid marker style," << arg;
        return;
    }
    m_markerStyle = arg;
    m_lineSpec.marker = arg;
    emit markerStyleChanged(arg);
    emit lineSpecChanged(m_lineSpec.toString());
}

qreal LinePlot::lineWidth() const
{
    return m_lineWidth;
}

void LinePlot::setLineWidth(qreal arg)
{
    if (m_lineWidth == arg) return;
    m_lineWidth = arg;
    emit lineWidthChanged(arg);
}

void LinePlot::setData(QList<qreal> xVals, QList<qreal> yVals)
{
    m_settingData = true;
    m_xData = xVals;
    m_yData = yVals;
    emit xDataChanged(xVals);
    emit yDataChanged(yVals);
    m_settingData = false;

    updateData();
}

void LinePlot::updateData()
{
    int xLen = m_xData.length(), yLen = m_yData.length();
    if ((xLen > 0 && xLen != yLen) || m_settingData)
        return; // Don't update until the data is good

    qreal minX = Inf, maxX = -Inf, minY = Inf, maxY = -Inf;
    for (int i=0; i<yLen; ++i) {
        qreal px = (xLen == 0) ? i : m_xData[i];
        qreal py = m_yData[i];
        minX = qMin(minX, px);
        maxX = qMax(maxX, px);
        minY = qMin(minY, py);
        maxY = qMax(maxY, py);
    }

    m_dataLimits.setLeft(minX);
    m_dataLimits.setTop(minY);
    m_dataLimits.setRight(maxX);
    m_dataLimits.setBottom(maxY);
    emit dataLimitsChanged(m_dataLimits);

    triggerUpdate();
}

LineSpec::LineSpec(QStringList specList)
{
    color = specList[0];
    style = specList[1];
    marker = specList[2];
}

LineSpec::LineSpec(QString spec)
{
    int lastInd = spec.length() - 1;

    marker = "";
    if (markers().contains(spec.mid(lastInd, 1), Qt::CaseInsensitive)) {
        marker = spec[lastInd];
        lastInd--;
    }

    color = "";
    if (colorMap.contains(spec.mid(lastInd, 1))) {
        color = spec[lastInd];
        lastInd--;
    }

//   style = "";
//    if (colors.contains(spec.left(lastInd + 1), Qt::CaseInsensitive))
    style = spec.left(lastInd + 1);

    //    m_specList = QStringList() << lineStyle << lineColor << markerSpec;
}

bool LineSpec::isValid() const
{
    if (!marker.isEmpty() && !markers().contains(marker, Qt::CaseInsensitive))
        return false;

    if (!color.isEmpty() && !colorMap.contains(color))
        return false;

    // Style MUST be defined to be valid
    if (style.isEmpty() || !styleMap.contains(style))
        return false;

    return true;
}

QStringList LineSpec::toList() const
{
    return QStringList() << style << color << marker;
}

QString LineSpec::toString() const
{
    return style + color + marker;
}


