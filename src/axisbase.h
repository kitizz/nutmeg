#ifndef AXIS_H
#define AXIS_H

#include <QQuickPaintedItem>
#include <QMultiMap>

#include "figurebase.h"
#include "linespec.h"
#include "locators.h"
#include "plotbase.h"
#include "nutmegobject.h"

#include <limits>
#define Inf std::numeric_limits<double>::infinity()

class AxisGrid;
class AxisMargins;
class AxisSpec;
class FigureBase;
class PlotBase;
class AxisBase : public QQuickPaintedItem, public NutmegObject
{
    Q_OBJECT
    Q_PROPERTY(AxisGrid* grid READ grid)
    Q_PROPERTY(AxisSpec* xAxis READ xAxis NOTIFY xAxisChanged)
    Q_PROPERTY(AxisSpec* yAxis READ yAxis NOTIFY yAxisChanged)
    Q_PROPERTY(QString handle READ handle WRITE setHandle NOTIFY handleChanged)

    Q_PROPERTY(FigureBase* figure READ figure WRITE setFigure NOTIFY figureChanged)
    Q_PROPERTY(QVariantMap plots READ plots NOTIFY plotsChanged)

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

public:
    explicit AxisBase(QQuickItem *parent = 0);
    ~AxisBase();

    Q_INVOKABLE qreal log_10(qreal v);
    Q_INVOKABLE QString formatReal(qreal num, int precision=3, int minMag=-11, int maxMag=-1);
    Q_INVOKABLE qreal offsetFromStd(qreal val, qreal std);

    void paint(QPainter *painter);

    FigureBase* figure() const;

    qreal minX() const;
    void setMinX(qreal arg, bool fix=true);
    qreal maxX() const;
    void setMaxX(qreal arg, bool fix=true);
    qreal minY() const;
    void setMinY(qreal arg, bool fix=true);
    qreal maxY() const;
    void setMaxY(qreal arg, bool fix=true);

    Q_INVOKABLE void offset(qreal x, qreal y);

    QString handle() const;
    void setHandle(QString arg);

    QVariantMap plots(); // For QML
    QList<PlotBase*> getPlotsByHandle(QString handle); // For C++
    Q_INVOKABLE QVariantList getPlotList();

    // NutmegObject interface
    Q_INVOKABLE QString map(QString prop);

    QRectF limits() const;
    void setLimits(QRectF arg, bool fix=true);
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

signals:
    void figureChanged(FigureBase* arg);
    void registerWithFigure(AxisBase* axis);
    void handleChanged(QString arg);
    void plotsChanged(QVariantMap arg);

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

public slots:
    void registerPlot(PlotBase *axis);
    void deregisterPlot(PlotBase *axis);
    void setFigure(FigureBase* arg);

protected slots:
    void updateFigure();
    void updateXAxis();
    void updateYAxis();
    void updatePlots();
    void updateLimits();
    void updateDataLimits();

private:
    void maintainAspectRatio(QRectF *lim);
    bool floatingLimits();
    FigureBase* m_figure;
    QMultiMap<QString, PlotBase*> m_plots;
    QVariantMap m_plotsVar;
    QVariantList m_plotsList;

    AxisGrid* m_grid;
    AxisSpec* m_xAxis;
    AxisSpec* m_yAxis;

    QString m_handle;
    qreal m_minX;
    qreal m_maxX;
    qreal m_minY;
    qreal m_maxY;
    bool m_minXFloat;
    bool m_maxXFloat;
    bool m_minYFloat;
    bool m_maxYFloat;

    bool m_destroying;
    bool m_settingLimits;
    QRectF m_limits;
    QRectF m_dataLimits;
    QList<qreal> m_yLimitRounding;

    // TODO: This really should go into a Util library....
    static qreal sign(qreal a);
    AxisMargins* m_margin;
    qreal m_aspectRatio;
    bool m_fitPlots;
};

class AxisGrid : public QObject, public NutmegObject
{
    Q_OBJECT
    Q_ENUMS(GridAxes)
    Q_PROPERTY(GridAxes axes READ axes WRITE setAxes NOTIFY axesChanged)
    Q_PROPERTY(LineSpec* majorLine READ majorLine)
    Q_PROPERTY(LineSpec* minorLine READ minorLine)

public:
    explicit AxisGrid(QObject* parent=0);
    enum GridAxes { None=0, XY=3, X=1, Y=2 };

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

class AxisSpec : public QObject, public NutmegObject
{
    Q_OBJECT
//    Q_PROPERTY(qreal pixelSize READ pixelSize NOTIFY pixelSizeChanged)
//    Q_PROPERTY(qreal min READ min NOTIFY minChanged)
//    Q_PROPERTY(qreal max READ max NOTIFY maxChanged)
    Q_ENUMS(TickDirection)
    Q_PROPERTY(QVariant majorTicks READ majorTicks WRITE setMajorTicks NOTIFY majorTicksChanged)
    Q_PROPERTY(QVariant minorTicks READ minorTicks WRITE setMinorTicks NOTIFY minorTicksChanged)
    Q_PROPERTY(qreal majorTickSize READ majorTickSize WRITE setMajorTickSize NOTIFY majorTickSizeChanged)
    Q_PROPERTY(qreal minorTickSize READ minorTickSize WRITE setMinorTickSize NOTIFY minorTickSizeChanged)
    Q_PROPERTY(LineSpec* majorLine READ majorLine)
    Q_PROPERTY(LineSpec* minorLine READ minorLine)
    Q_PROPERTY(TickDirection tickDir READ tickDir WRITE setTickDir NOTIFY tickDirChanged)
    Q_PROPERTY(bool inverted READ inverted WRITE setInverted NOTIFY invertedChanged)

public:
    explicit AxisSpec(QObject *parent=0);
//    explicit AxisSpec(QString sizeProperty, QObject *parent);
    enum TickDirection { Off=0, In=1, Out=2, InOut=3 };

    QVariantList majorTicks() const;
    void setMajorTicks(QVariant arg);

    QVariantList minorTicks() const;
    void setMinorTicks(QVariant arg);

    LineSpec* majorLine() const;
    LineSpec* minorLine() const;

    qreal pixelSize() const;
    qreal min() const;
    qreal max() const;

    bool inverted() const;
    void setInverted(bool arg);

    TickDirection tickDir() const;
    void setTickDir(TickDirection arg);

    qreal majorTickSize() const;
    void setMajorTickSize(qreal arg);

    qreal minorTickSize() const;
    void setMinorTickSize(qreal arg);

signals:
    void majorTicksChanged(QList<qreal> arg);
    void minorTicksChanged(QList<qreal> arg);

    void pixelSizeChanged(qreal arg);
    void minChanged(qreal arg);
    void maxChanged(qreal arg);

    void invertedChanged(bool arg);

    void ticksChanged(AxisSpec* arg);

    void tickDirChanged(TickDirection arg);

    void majorTickSizeChanged(qreal arg);

    void minorTickSizeChanged(qreal arg);

protected slots:
    void updateMajor();
    void updateMinor();

private:
    void setPixelSize(qreal arg);
    void setMin(qreal arg);
    void setMax(qreal arg);

    Locator* m_majorTicks;
    Locator* m_minorTicks;
    QVariant m_majorTicksVar;
    QVariant m_minorTicksVar;
    bool m_ownMajorTicks;
    bool m_ownMinorTicks;
    LineSpec* m_majorLine;
    LineSpec* m_minorLine;
    qreal m_pixelSize;
    qreal m_min;
    qreal m_max;

    friend class AxisBase;
    bool m_inverted;
    TickDirection m_tickDir;
    qreal m_majorTickSize;
    qreal m_minorTickSize;
};

class AxisMargins : public QObject
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

#endif // AXIS_H
