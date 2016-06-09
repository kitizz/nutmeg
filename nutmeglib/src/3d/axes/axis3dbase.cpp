#ifdef SUPPORT_3D
#include "axis3dbase.h"

#include "../plots/plot3dbase.h"

#include "../../util/util.h"

Axis3DBase::Axis3DBase(QQuickItem *parent)
    : AxisBase(parent)
    , m_xAxis(new AxisSpec(parent))
    , m_yAxis(new AxisSpec(parent))
    , m_zAxis(new AxisSpec(parent))
    , m_title("")
    , m_titleFont(QFont())
    , m_titleColor(QColor("black"))
    , m_rootEntity(0)
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

QEntity *Axis3DBase::rootEntity() const
{
    return m_rootEntity;
}

void Axis3DBase::setRootEntity(QEntity *rootEntity)
{
    if (m_rootEntity == rootEntity)
        return;

    m_rootEntity = rootEntity;
    qDebug() << "Setting root entity" << rootEntity;

    updatePlotEntities();
//    dumpInfo();
    emit rootEntityChanged(rootEntity);
}

void Axis3DBase::register3DPlot(Plot3DBase *plot)
{
    qDebug() << "Registering 3D plot:" << plot->handle();
    m_plots.insert(plot->handle(), plot);
    plot->entity()->setParent(rootEntity());

    AxisBase::registerPlot(plot);
//    dumpInfo();
}

void Axis3DBase::deregister3DPlot(Plot3DBase *plot)
{
    m_plots.remove(plot->handle());
    AxisBase::deregisterPlot(plot);
}

void Axis3DBase::updateDataLimits()
{
    QList<qreal> lim;
    lim << Inf << -Inf << Inf << -Inf << Inf << -Inf;

    foreach (auto plot, m_plots.values()) {
        QList<qreal> lims = plot->dataLimits();
        for (int i=0; i<6; i+=2) {
            lim[i] = qMin(lim[i], lims[i]);
            lim[i + 1] = qMax(lim[i + 1], lims[i + 1]);
        }
    }

    setMinX(lim[0]);
    setMaxX(lim[1]);
    setMinY(lim[2]);
    setMaxY(lim[3]);
    setMinZ(lim[4]);
    setMaxZ(lim[5]);
}

void Axis3DBase::setMinX(qreal minX)
{
    if (m_minX == minX)
        return;

    m_minX = minX;
    emit minXChanged(minX);
}

void Axis3DBase::setMaxX(qreal maxX)
{
    if (m_maxX == maxX)
        return;

    m_maxX = maxX;
    emit maxXChanged(maxX);
}

void Axis3DBase::setMinY(qreal minY)
{
    if (m_minY == minY)
        return;

    m_minY = minY;
    emit minYChanged(minY);
}

void Axis3DBase::setMaxY(qreal maxY)
{
    if (m_maxY == maxY)
        return;

    m_maxY = maxY;
    emit maxYChanged(maxY);
}

void Axis3DBase::setMinZ(qreal minZ)
{
    if (m_minZ == minZ)
        return;

    m_minZ = minZ;
    emit minZChanged(minZ);
}

void Axis3DBase::setMaxZ(qreal maxZ)
{
    if (m_maxZ == maxZ)
        return;

    m_maxZ = maxZ;
    emit maxZChanged(maxZ);
}

void Axis3DBase::updatePlotEntities()
{
    foreach (Plot3DBase *plot, m_plots.values()) {
        plot->entity()->setParent(m_rootEntity);
    }
}

void Axis3DBase::dumpInfo()
{
    QEntity *par = m_rootEntity;
    while (par->parentEntity()) {
        par = par->parentEntity();
    }
    qDebug() << "Dumping root entity:" << par;
    dumpTree(par, 0);
}

void Axis3DBase::dumpTree(QNode* e, int level)
{
    qDebug() << QString("-").repeated(level).toLocal8Bit().constData() << e;
    foreach(QNode *n, e->childrenNodes()) {
        dumpTree(n, level + 1);
    }
}

qreal Axis3DBase::minX() const
{
    return m_minX;
}

qreal Axis3DBase::maxX() const
{
    return m_maxX;
}

qreal Axis3DBase::minY() const
{
    return m_minY;
}

qreal Axis3DBase::maxY() const
{
    return m_maxY;
}

qreal Axis3DBase::minZ() const
{
    return m_minZ;
}

qreal Axis3DBase::maxZ() const
{
    return m_maxZ;
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
