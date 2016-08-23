#ifndef AXIS2DBASE_H
#define AXIS2DBASE_H

#include <QQuickPaintedItem>
#include <QMultiMap>

#include "../../common/axisbase.h"
#include "../../common/linespec.h"
#include "../../common/locators.h"
#include "../plots/plot2dbase.h"
#include "../../common/nutmegobject.h"
#include "../../common/axisspec.h"

class AxisGrid;
class AxisMargins;
class AxisSpec;
class FigureBase;
class Plot2DBase;
class AxisBase;
class AxisCanvas2D;
class NUTMEGLIB_EXPORT Axis2DBase : public AxisBase
{
    Q_OBJECT
    Q_PROPERTY(AxisGrid* grid READ grid)
    Q_PROPERTY(AxisSpec* xAxis READ xAxis NOTIFY xAxisChanged)
    Q_PROPERTY(AxisSpec* yAxis READ yAxis NOTIFY yAxisChanged)  

    Q_PROPERTY(QRectF limits READ limits WRITE setLimits NOTIFY limitsChanged RESET resetLimits)

    Q_PROPERTY(qreal minX READ minX WRITE setMinX NOTIFY minXChanged)
    Q_PROPERTY(qreal maxX READ maxX WRITE setMaxX NOTIFY maxXChanged)
    Q_PROPERTY(qreal minY READ minY WRITE setMinY NOTIFY minYChanged)
    Q_PROPERTY(qreal maxY READ maxY WRITE setMaxY NOTIFY maxYChanged)
    Q_PROPERTY(QRectF dataLimits READ dataLimits NOTIFY dataLimitsChanged)

    Q_PROPERTY(AxisMargins* margin READ margin NOTIFY marginChanged)
    Q_PROPERTY(QList<qreal> yLimitRounding READ yLimitRounding WRITE setYLimitRounding NOTIFY yLimitRoundingChanged)

    Q_PROPERTY(qreal aspectRatio READ aspectRatio WRITE setAspectRatio NOTIFY aspectRatioChanged)
    Q_PROPERTY(bool fitPlots READ fitPlots WRITE setFitPlots NOTIFY fitPlotsChanged)
    Q_PROPERTY(QString shareX READ shareX WRITE setShareX NOTIFY shareXChanged)
    Q_PROPERTY(QString shareY READ shareY WRITE setShareY NOTIFY shareYChanged)

public:
    explicit Axis2DBase(QQuickItem *parent = 0);

    virtual Renderer *createRenderer() const;

    Q_INVOKABLE qreal log_10(qreal v);
    Q_INVOKABLE QString formatReal(qreal num, int precision=3, int minMag=-11, int maxMag=-1);
    Q_INVOKABLE qreal offsetFromStd(qreal val, qreal std);

    qreal minX() const;
    void setMinX(qreal arg, bool fix=true, bool updateAxis=true);
    qreal maxX() const;
    void setMaxX(qreal arg, bool fix=true, bool updateAxis=true);
    qreal minY() const;
    void setMinY(qreal arg, bool fix=true, bool updateAxis=true);
    qreal maxY() const;
    void setMaxY(qreal arg, bool fix=true, bool updateAxis=true);

    Q_INVOKABLE void offset(qreal x, qreal y);

    QRectF limits() const;
    void resetLimits();

    QRectF dataLimits() const;
    QList<qreal> yLimitRounding() const;
    void setYLimitRounding(QList<qreal> arg);

    AxisGrid* grid() const;
    AxisSpec* xAxis() const;
    AxisSpec* yAxis() const;

    AxisMargins* margin() const;
    qreal aspectRatio() const;
    void setAspectRatio(qreal arg);

    bool fitPlots() const;
    void setFitPlots(bool arg);

    QString shareX() const;
    void setShareX(QString arg);

    QString shareY() const;
    void setShareY(QString arg);

signals:
    void minXChanged(qreal arg);
    void maxXChanged(qreal arg);
    void minYChanged(qreal arg);
    void maxYChanged(qreal arg);

    void limitsChanged(QRectF arg);
    void dataLimitsChanged(QRectF arg);

    void marginChanged(AxisMargins* margin);
    void yLimitRoundingChanged(QList<qreal> arg);

    void xAxisChanged(AxisSpec* arg);
    void yAxisChanged(AxisSpec* arg);

    void aspectRatioChanged(qreal arg);
    void fitPlotsChanged(bool arg);
    void shareXChanged(QString arg);
    void shareYChanged(QString arg);

public slots:
    void setLimits(QRectF arg, bool fix=true, bool shareUpdate=false);
    virtual void print(QPainter *painter);
    virtual void triggerRedraw();
    Q_INVOKABLE QRectF maintainAspectRatio(QRectF lim, QPointF center=QPointF(0.5, 0.5));

protected slots:
    void updateXAxis();
    void updateYAxis();
    void updateLimits();
    void updateDataLimits();

private:
    bool floatingLimits();

    AxisGrid* m_grid;
    AxisSpec* m_xAxis;
    AxisSpec* m_yAxis;

    qreal m_minX;
    qreal m_maxX;
    qreal m_minY;
    qreal m_maxY;
    bool m_minXFloat;
    bool m_maxXFloat;
    bool m_minYFloat;
    bool m_maxYFloat;

    bool m_settingLimits;
    QRectF m_limits;
    QRectF m_dataLimits;
    QList<qreal> m_yLimitRounding;

    // TODO: This really should go into a Util library....
    static qreal sign(qreal a);
    AxisMargins* m_margin;
    qreal m_aspectRatio;
    bool m_fitPlots;
    QString m_shareX;
    QString m_shareY;
};

class NUTMEGLIB_EXPORT AxisGrid : public QObject, public NutmegObject
{
    Q_OBJECT
    Q_ENUMS(GridAxes)
    Q_PROPERTY(GridAxes axes READ axes WRITE setAxes NOTIFY axesChanged)
    Q_PROPERTY(LineSpec* majorLine READ majorLine)
    Q_PROPERTY(LineSpec* minorLine READ minorLine)

public:
    explicit AxisGrid(QObject* parent=0);
    enum GridAxes { None=0, XY=3, X=1, Y=2 };

    // Interface to NutmegObject
    Q_INVOKABLE QString mapProperty(const QString &prop);

    GridAxes axes() const;
    void setAxes(GridAxes arg);

    LineSpec* majorLine() const;
    LineSpec* minorLine() const;

signals:
    void axesChanged(GridAxes arg);

private:
    GridAxes m_axis;

    LineSpec* m_majorLine;
    LineSpec* m_minorLine;
};

class NUTMEGLIB_EXPORT AxisMargins : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal left READ left WRITE setLeft NOTIFY leftChanged)
    Q_PROPERTY(qreal right READ right WRITE setRight NOTIFY rightChanged)
    Q_PROPERTY(qreal top READ top WRITE setTop NOTIFY topChanged)
    Q_PROPERTY(qreal bottom READ bottom WRITE setBottom NOTIFY bottomChanged)

public:
    explicit AxisMargins(QObject *parent = 0);
    qreal left() const;
    void setLeft(qreal arg);

    qreal right() const;
    void setRight(qreal arg);

    qreal top() const;
    void setTop(qreal arg);

    qreal bottom() const;
    void setBottom(qreal arg);

signals:
    void leftChanged(qreal arg);
    void rightChanged(qreal arg);
    void topChanged(qreal arg);
    void bottomChanged(qreal arg);

private:
    qreal m_left;
    qreal m_right;
    qreal m_top;
    qreal m_bottom;
};

#endif // AXIS2DBASE_H
