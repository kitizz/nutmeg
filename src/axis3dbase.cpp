#ifdef SUPPORT_3D
#include "axis3dbase.h"

Axis3DBase::Axis3DBase(QQuickItem *parent)
    : AxisBase(parent)
    , m_xAxis(new AxisSpec(parent))
    , m_yAxis(new AxisSpec(parent))
    , m_zAxis(new AxisSpec(parent))
    , m_title("")
    , m_titleFont(QFont())
    , m_titleColor(QColor("black"))
{
    // Initialise fonts
    m_titleFont.setBold(true);

    // Register properties available through the API
    QMap<QString,QString> props;
    props.insert("xAxis", "xAxis");
    props.insert("yAxis", "yAxis");
    props.insert("zAxis", "zAxis");
    props.insert("title", "title");
    registerProperties(props);
    connect(this, &QQuickItem::parentChanged, this, &Axis3DBase::updateFigure);

    // Set up axes
    m_xAxis->setObjectName("XAxis");
//    m_xAxis->setMin(minX());
//    m_xAxis->setMax(maxX());

    m_yAxis->setObjectName("YAxis");
//    m_yAxis->setMin(minY());
//    m_yAxis->setMax(maxY());

    m_zAxis->setObjectName("YAxis");
//    m_zAxis->setMin(minY());
//    m_zAxis->setMax(maxY());
}

QString Axis3DBase::title() const
{
    return m_title;
}

QFont Axis3DBase::titleFont() const
{
    return m_titleFont;
}

QColor Axis3DBase::titleColor() const
{
    return m_titleColor;
}

void Axis3DBase::setTitle(QString title)
{
    if (m_title == title)
        return;

    m_title = title;
    emit titleChanged(title);
}

void Axis3DBase::setTitleFont(QFont titleFont)
{
    if (m_titleFont == titleFont)
        return;

    m_titleFont = titleFont;
    emit titleFontChanged(titleFont);
}

void Axis3DBase::setTitleColor(QColor titleColor)
{
    if (m_titleColor == titleColor)
        return;

    m_titleColor = titleColor;
    emit titleColorChanged(titleColor);
}

AxisSpec *Axis3DBase::xAxis() const
{
    return m_xAxis;
}

AxisSpec *Axis3DBase::yAxis() const
{
    return m_yAxis;
}

AxisSpec *Axis3DBase::zAxis() const
{
    return m_zAxis;
}

#endif
