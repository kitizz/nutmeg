#ifdef SUPPORT_3D
#ifndef AXIS3DBASE_H
#define AXIS3DBASE_H

#include <QQuickItem>

#include "../../common/nutmegobject.h"
#include "../../common/axisspec.h"
#include "../../common/figurebase.h"
#include "../../common/axisbase.h"

#include <Qt3DCore/QEntity>

class Plot3DBase;
class NUTMEGLIB_EXPORT Axis3DBase : public AxisBase
{
    Q_OBJECT
    Q_PROPERTY(AxisSpec* xAxis READ xAxis NOTIFY xAxisChanged)
    Q_PROPERTY(AxisSpec* yAxis READ yAxis NOTIFY yAxisChanged)
    Q_PROPERTY(AxisSpec* zAxis READ zAxis NOTIFY zAxisChanged)
    Q_PROPERTY(Qt3DCore::QEntity* rootEntity READ rootEntity WRITE setRootEntity NOTIFY rootEntityChanged)

    Q_PROPERTY(qreal minX READ minX WRITE setMinX NOTIFY minXChanged)
    Q_PROPERTY(qreal maxX READ maxX WRITE setMaxX NOTIFY maxXChanged)
    Q_PROPERTY(qreal minY READ minY WRITE setMinY NOTIFY minYChanged)
    Q_PROPERTY(qreal maxY READ maxY WRITE setMaxY NOTIFY maxYChanged)
    Q_PROPERTY(qreal minZ READ minZ WRITE setMinZ NOTIFY minZChanged)
    Q_PROPERTY(qreal maxZ READ maxZ WRITE setMaxZ NOTIFY maxZChanged)
    Q_PROPERTY(QRectF dataLimits READ dataLimits NOTIFY dataLimitsChanged)

public:
    explicit Axis3DBase(QQuickItem *parent = 0);

    AxisSpec *xAxis() const;
    AxisSpec *yAxis() const;
    AxisSpec *zAxis() const;

    QString title() const;
    void setTitle(QString title);

    QFont titleFont() const;
    void setTitleFont(QFont titleFont);

    QColor titleColor() const;
    void setTitleColor(QColor titleColor);

    Qt3DCore::QEntity* rootEntity() const;
    void dumpTree(Qt3DCore::QNode *e, int level);

    qreal minX() const;
    qreal maxX() const;
    qreal minY() const;
    qreal maxY() const;
    qreal minZ() const;
    qreal maxZ() const;

    QRectF dataLimits() const
    {
        return m_dataLimits;
    }

signals:
    void xAxisChanged(AxisSpec* xAxis);
    void yAxisChanged(AxisSpec* yAxis);
    void zAxisChanged(AxisSpec* zAxis);

    void titleChanged(QString title);
    void titleFontChanged(QFont titleFont);
    void titleColorChanged(QColor titleColor);

    void rootEntityChanged(Qt3DCore::QEntity* rootEntity);

    void minXChanged(qreal minX);
    void maxXChanged(qreal maxX);
    void minYChanged(qreal minY);
    void maxYChanged(qreal maxY);
    void minZChanged(qreal minZ);
    void maxZChanged(qreal maxZ);

    void dataLimitsChanged(QRectF dataLimits);

public slots:
    void setRootEntity(Qt3DCore::QEntity* rootEntity);
    virtual void register3DPlot(Plot3DBase *plot);
    virtual void deregister3DPlot(Plot3DBase *plot);

    void updateDataLimits();

    void setMinX(qreal minX);
    void setMaxX(qreal maxX);
    void setMinY(qreal minY);
    void setMaxY(qreal maxY);
    void setMinZ(qreal minZ);
    void setMaxZ(qreal maxZ);


protected:
    void updatePlotEntities();
    void dumpInfo();

private:
    AxisSpec* m_xAxis;
    AxisSpec* m_yAxis;
    AxisSpec* m_zAxis;

    QString m_title;
    QFont m_titleFont;
    QColor m_titleColor;

    QMap<QString, Plot3DBase*> m_plots;
    Qt3DCore::QEntity* m_rootEntity;
    qreal m_minX;
    qreal m_maxX;
    qreal m_minY;
    qreal m_maxY;
    qreal m_minZ;
    qreal m_maxZ;
    QRectF m_dataLimits;
};

#endif // AXIS3DBASE_H
#endif
