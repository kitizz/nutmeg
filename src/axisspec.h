#ifndef AXISSPEC_H
#define AXISSPEC_H

#include <QObject>
#include <QFont>

#include "nutmegobject.h"
#include "linespec.h"
#include "locators.h"

class AxisSpec : public QObject, public NutmegObject
{
    Q_OBJECT
//    Q_PROPERTY(qreal pixelSize READ pixelSize NOTIFY pixelSizeChanged)
//    Q_PROPERTY(qreal min READ min NOTIFY minChanged)
//    Q_PROPERTY(qreal max READ max NOTIFY maxChanged)
    Q_ENUMS(TickDirection)
    Q_PROPERTY(QVariant majorTicks READ majorTicks WRITE setMajorTicks NOTIFY majorTicksChanged)
    Q_PROPERTY(QVariant minorTicks READ minorTicks WRITE setMinorTicks NOTIFY minorTicksChanged)
    Q_PROPERTY(QStringList majorTickLabels READ majorTickLabels NOTIFY majorTickLabelsChanged)
    Q_PROPERTY(QStringList minorTickLabels READ minorTickLabels NOTIFY minorTickLabelsChanged)
    Q_PROPERTY(qreal majorTickSize READ majorTickSize WRITE setMajorTickSize NOTIFY majorTickSizeChanged)
    Q_PROPERTY(qreal minorTickSize READ minorTickSize WRITE setMinorTickSize NOTIFY minorTickSizeChanged)
    Q_PROPERTY(LineSpec* majorLine READ majorLine)
    Q_PROPERTY(LineSpec* minorLine READ minorLine)
    Q_PROPERTY(TickDirection tickDir READ tickDir WRITE setTickDir NOTIFY tickDirChanged)

    Q_PROPERTY(int tickPrecision READ tickPrecision WRITE setTickPrecision NOTIFY tickPrecisionChanged)

    Q_PROPERTY(QColor tickTextColor READ tickTextColor WRITE setTickTextColor NOTIFY tickTextColorChanged)
    Q_PROPERTY(QFont tickFont READ tickFont WRITE setTickFont NOTIFY tickFontChanged)

    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged)
    Q_PROPERTY(QColor labelColor READ labelColor WRITE setLabelColor NOTIFY labelColorChanged)
    Q_PROPERTY(QFont labelFont READ labelFont WRITE setLabelFont NOTIFY labelFontChanged)

    Q_PROPERTY(bool inverted READ inverted WRITE setInverted NOTIFY invertedChanged)

public:
    explicit AxisSpec(QObject *parent=0);
//    explicit AxisSpec(QString sizeProperty, QObject *parent);
    enum TickDirection { Off=0, In=1, Out=2, InOut=3 };

    // NutmegObject interface
    Q_INVOKABLE QString mapProperty(const QString &prop);

    QVariantList majorTicks() const;
    QList<qreal> majorTicksReal() const;
    void setMajorTicks(QVariant arg);

    QVariantList minorTicks() const;
    QList<qreal> minorTicksReal() const;
    void setMinorTicks(QVariant arg);

    QStringList majorTickLabels() const;
    QStringList minorTickLabels() const;

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

    QColor tickTextColor() const;
    void setTickTextColor(QColor arg);

    QFont tickFont() const;
    void setTickFont(QFont arg);

    QString label() const;
    void setLabel(QString arg);

    QColor labelColor() const;
    void setLabelColor(QColor arg);

    QFont labelFont() const;
    void setLabelFont(QFont arg);

    int tickPrecision() const;
    void setTickPrecision(int arg);

public slots:

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

    void tickTextColorChanged(QColor arg);
    void tickFontChanged(QFont arg);

    void labelChanged(QString arg);
    void labelColorChanged(QColor arg);
    void labelFontChanged(QFont arg);

    void majorTickLabelsChanged(QStringList arg);
    void minorTickLabelsChanged(QStringList arg);

    void tickPrecisionChanged(int arg);

protected slots:
    void updateMajor();
    void updateMinor();

private:
    void setPixelSize(qreal arg);
    void setMin(qreal arg);
    void setMax(qreal arg);
    void setLimits(qreal min, qreal max);

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
    friend class Axis2DBase;
    bool m_inverted;
    TickDirection m_tickDir;
    qreal m_majorTickSize;
    qreal m_minorTickSize;
    int m_tickPrecision;
    QColor m_tickTextColor;
    QFont m_tickFont;
    QString m_label;
    QColor m_labelColor;
    QFont m_labelFont;
};

#endif // AXISSPEC_H
