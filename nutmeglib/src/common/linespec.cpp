#include "linespec.h"

QMap<QString,QColor> LineSpec::colorMap = LineSpec::generateColorMap();
QMap<QString, Qt::PenStyle> LineSpec::styleMap = LineSpec::generateStyleMap();

LineSpec::LineSpec(QObject *parent)
    : QObject(parent), NutmegObject(this)
//    , m_spec(QString("-k"))
    , m_color(Qt::black)
    , m_colorStr(QString("k"))
    , m_style(QString("-"))
    , m_width(1)
    , m_marker(QString())
{
    QMap<QString, QString> props;
    props["spec"] = "spec";
    props["color"] = "color";
    props["style"] = "style";
    props["width"] = "width";
    props["marker"] = "marker";
    registerProperties(props);
}

LineSpec::LineSpec(QString spec, QObject *parent)
    : LineSpec(parent)
{
    setSpec(spec);
}

QStringList LineSpec::toList() const
{
    return QStringList() << m_style << m_colorStr << m_marker;
}

QString LineSpec::toString() const
{
    return m_style + m_colorStr + m_marker;
}

QString LineSpec::spec() const
{
    return toString();
}

void LineSpec::setSpec(QString arg)
{
    if (spec() == arg) return;

    int lastInd = arg.length() - 1;

    if (markers().contains(arg.mid(lastInd, 1))) {
        setMarker(arg.mid(lastInd, 1));
        lastInd--;
    }

    if (colorMap.contains(arg.mid(lastInd, 1))) {
        setColor(colorMap[arg.mid(lastInd, 1)]);
        lastInd--;
    }

    QString style = arg.left(lastInd + 1);
    if (!style.isEmpty())
        setStyle(style);

    emit specChanged(arg);
}

QColor LineSpec::color() const
{
    return m_color;
}

void LineSpec::setColor(QColor arg)
{
    if (m_color == arg) return;
    m_color = arg;
    m_colorStr = "";

    // See if the color corresponds to any "rgbcmykw" string value
    QMap<QString, QColor>::iterator i;
    for (i=colorMap.begin(); i != colorMap.end(); ++i) {
        if (i.value() != arg) continue;
        m_colorStr = i.key();
        break;
    }

    emit colorChanged(arg);
}

QString LineSpec::style() const
{
    return m_style;
}

// TODO: Make a single doc for linespec
/*!
 * @brief LienSpec::setStyle
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
void LineSpec::setStyle(QString arg)
{
    if (m_style == arg) return;
    // TODO: Check style is valid in painter...
    m_style = arg;
    emit styleChanged(arg);
}

qreal LineSpec::width() const
{
    return m_width;
}

void LineSpec::setWidth(qreal arg)
{
    if (m_width == arg) return;
    m_width = arg;
    emit widthChanged(arg);
}

QString LineSpec::marker() const
{
    return m_marker;
}

void LineSpec::setMarker(QString arg)
{
    if (m_marker == arg) return;
    // TODO: Check that the marker is valid
    m_marker = arg;
    emit markerChanged(arg);
}
