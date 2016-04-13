#ifndef LOCATORS_H
#define LOCATORS_H

#include "../nutmeglibdecl.h"
#include <QObject>
#include <QStringList>

class NUTMEGLIB_EXPORT Locator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<qreal> locations READ locations WRITE setLocations NOTIFY locationsChanged)
    Q_PROPERTY(QStringList locationLabels READ locationLabels WRITE setLocationLabels NOTIFY locationLabelsChanged)
    Q_PROPERTY(qreal start READ start WRITE setStart NOTIFY startChanged)
    Q_PROPERTY(qreal end READ end WRITE setEnd NOTIFY endChanged)
    Q_PROPERTY(qreal pixelSize READ pixelSize WRITE setPixelSize NOTIFY pixelSizeChanged)
    Q_PROPERTY(QList<qreal> multiples READ multiples WRITE setMultiples NOTIFY multiplesChanged)

public:
    explicit Locator(QObject *parent = 0);

    QList<qreal> locations() const;
    void setLocations(QList<qreal> arg);

    qreal start() const;
    void setStart(qreal arg);

    qreal end() const;
    void setEnd(qreal arg);

    void setLimits(qreal start, qreal end);

    qreal pixelSize() const;
    void setPixelSize(qreal arg);

    QList<qreal> multiples() const;
    void setMultiples(QList<qreal> arg);

    QStringList locationLabels() const;
    void setLocationLabels(QStringList arg);

    void setLocationsAndLabels(QList<qreal> locs, QStringList labels);

signals:
    void locationsChanged(QList<qreal> arg);
    void startChanged(qreal arg);
    void endChanged(qreal arg);
    void pixelSizeChanged(qreal arg);
    void multiplesChanged(QList<qreal> arg);

    void locationLabelsChanged(QStringList arg);

protected slots:

public slots:
    virtual void updateLocator() {}

private:
    QList<qreal> m_locations;
    qreal m_start;
    qreal m_end;
    qreal m_pixelSize;
    QList<qreal> m_multiples;
    qreal m_scale;
    QStringList m_locationLabels;
};

class NUTMEGLIB_EXPORT AutoLocator : public Locator
{
    Q_OBJECT

    Q_PROPERTY(qreal density READ density WRITE setDensity NOTIFY densityChanged)

public:
    explicit AutoLocator(QObject *parent = 0);
    explicit AutoLocator(qreal density, QObject *parent=0);
    qreal density() const;
    void setDensity(qreal arg);

signals:
    void densityChanged(qreal arg);

public slots:
    virtual void updateLocator();

private:
    qreal roundAt(qreal n, qreal f);
    qreal m_density;
};

class NUTMEGLIB_EXPORT SpacedLocator : public Locator
{
    Q_OBJECT
    Q_PROPERTY(qreal spacing READ spacing WRITE setSpacing NOTIFY spacingChanged)

public:
    explicit SpacedLocator(QObject *parent=0);
    explicit SpacedLocator(qreal spacing, QObject *parent=0);

    qreal spacing() const;
    void setSpacing(qreal arg);

signals:
    void spacingChanged(qreal arg);

public slots:
    virtual void updateLocator();

private:
    qreal m_spacing;
};

class NUTMEGLIB_EXPORT HardLocator : public Locator
{
    Q_OBJECT
    Q_PROPERTY(QList<qreal> hardLocations READ hardLocations WRITE setHardLocations NOTIFY hardLocationsChanged)

public:
    explicit HardLocator(QObject *parent = 0);
    explicit HardLocator(QList<qreal> hardLocs, QObject *parent = 0);

    QList<qreal> hardLocations() const;
    void setHardLocations(QList<qreal> arg);

signals:

    void hardLocationsChanged(QList<qreal> arg);

public slots:
    virtual void updateLocator();

private:
    QList<qreal> m_hardLocations;
};

class NUTMEGLIB_EXPORT LabelLocator : public HardLocator
{
    Q_OBJECT
    Q_PROPERTY(QStringList labels READ labels WRITE setLabels NOTIFY labelsChanged)

public:
    explicit LabelLocator(QObject *parent = 0);
    explicit LabelLocator(QStringList labels, QList<qreal> hardLocs, QObject *parent = 0);

    QStringList labels() const;
    void setLabels(QStringList arg);

signals:

    void labelsChanged(QStringList arg);

public slots:
    virtual void updateLocator();

private:
    QStringList m_labels;
};

#endif // LOCATORS_H
