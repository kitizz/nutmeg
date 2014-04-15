#include "locators.h"
#include <math.h>
#include <QtCore/qmath.h>
#include <QDebug>

Locator::Locator(QObject *parent)
    : QObject(parent)
    , m_locations(QList<qreal>())
    , m_start(0)
    , m_end(1)
    , m_pixelSize(1)
    , m_multiples(QList<qreal>())
{
    // This default covers other magnitudes like 0.1, 0.5, 10, 20, 500, etc...
    m_multiples << 1 << 2 << 5;
    // Connect start, end, pixelSize with updateScale...
    connect(this, &Locator::startChanged, this, &Locator::updateLocator);
    connect(this, &Locator::endChanged, this, &Locator::updateLocator);
    connect(this, &Locator::pixelSizeChanged, this, &Locator::updateLocator);
}

QList<qreal> Locator::locations() const
{
    return m_locations;
}

void Locator::setLocations(QList<qreal> arg)
{
    if (m_locations == arg) return;
    m_locations = arg;
    emit locationsChanged(arg);
}

qreal Locator::start() const
{
    return m_start;
}

void Locator::setStart(qreal arg)
{
    if (m_start == arg) return;
    m_start = arg;
    emit startChanged(arg);
}

qreal Locator::end() const
{
    return m_end;
}

void Locator::setEnd(qreal arg)
{
    if (m_end == arg) return;
    m_end = arg;
    emit endChanged(arg);
}

qreal Locator::pixelSize() const
{
    return m_pixelSize;
}

void Locator::setPixelSize(qreal arg)
{
    if (m_pixelSize == arg) return;
    m_pixelSize = arg;
    emit pixelSizeChanged(arg);
}

QList<qreal> Locator::multiples() const
{
    return m_multiples;
}

void Locator::setMultiples(QList<qreal> arg)
{
    if (m_multiples == arg) return;
    m_multiples = arg;
    emit multiplesChanged(arg);
}


//-------------------------------------------------------------------
//          AutoLocator
//-------------------------------------------------------------------

AutoLocator::AutoLocator(QObject *parent)
    : Locator(parent)
    , m_density(50)
{
}

AutoLocator::AutoLocator(qreal density, QObject *parent)
    : Locator(parent)
    , m_density(density)
{
    updateLocator();
}

/*!
 * \brief AutoLocator::density
 * \return The preferred density of the locator in pixels/tick. By default this is 50.
 */
qreal AutoLocator::density() const
{
    return m_density;
}

void AutoLocator::setDensity(qreal arg)
{
    if (m_density == arg) return;
    m_density = arg;
    updateLocator();
    emit densityChanged(arg);
}

void AutoLocator::updateLocator()
{
    /* We want to find the multiple (n), and the magnitude (m) of the multiples that
     * get a tick density closest to the preferred density. The error can be
     * expressed in 2 ways:
     * density = scale*n*10^m + e   (1), OR
     * density = scale*n*10^(m+a)   (2)
     * Solving gives:
     * e = density*(1 - 10^(-a))
     * We want to minimise `e`
     */
    // We can find `a` using (1)
    qreal scale = pixelSize()/(end() - start());
    if (scale <= 0) setLocations(QList<qreal>());

    qreal bestA = m_density, bestMult = 0;
    foreach (qreal n, multiples()) {
        qreal m_a = log10(m_density/(scale*n));
        qreal m = roundAt(m_a, 0.7); // Approx. log10(5).
        qreal a = qAbs(m_a - m);
        if (a < bestA) {
            bestA = a;
            bestMult = n*qPow(10, m);
        }
    }

    if (bestMult == 0) return; // No good...

    // Round the start value up to the next valid value
    QList<qreal> newLocs;
    newLocs << qCeil(start()/bestMult)*bestMult;
    while (newLocs.last() <= end())
        newLocs << newLocs.last() + bestMult;
    // The last one is larger than the end value.
    newLocs.removeLast();

    // If only one value remains, set it to the start and end values
    if (newLocs.length() < 2) {
        newLocs.clear();
        newLocs << start() << end();
    }

    setLocations(newLocs);
}

qreal AutoLocator::roundAt(qreal n, qreal f)
{
    qreal nInt = qFloor(n);
    qreal nFrac = n - nInt;
//    if (n<0) nFrac = 1 - nFrac;
    nFrac = nFrac < f ? 0 : 1;

    return nInt + nFrac;
}

// ------------------------------------------------------------------
//              HardLocator
// ------------------------------------------------------------------
HardLocator::HardLocator(QObject *parent)
    : Locator(parent)
    , m_hardLocations(QList<qreal>())
{
}

HardLocator::HardLocator(QList<qreal> hardLocs, QObject *parent)
    : Locator(parent)
    , m_hardLocations(hardLocs)
{
    updateLocator();
}

void HardLocator::setHardLocations(QList<qreal> arg)
{
    if (m_hardLocations == arg) return;
    m_hardLocations = arg;
    updateLocator();
    emit hardLocationsChanged(arg);
}

void HardLocator::updateLocator()
{
//    qDebug() << "Updating HardLocator" << start() << end() << m_hardLocations;
    QList<qreal> newLocs;
    foreach (qreal val, m_hardLocations) {
        if (val >= start() && val <= end())
            newLocs << val;
    }
    setLocations(newLocs);
}

QList<qreal> HardLocator::hardLocations() const
{
    return m_hardLocations;
}

// ------------------------------------------------------------------
//          SpacedLocator
// ------------------------------------------------------------------
SpacedLocator::SpacedLocator(QObject *parent)
    : Locator(parent)
    , m_spacing(1)
{
}

SpacedLocator::SpacedLocator(qreal spc, QObject *parent)
    : Locator(parent)
    , m_spacing(spc)
{
    updateLocator();
}

void SpacedLocator::setSpacing(qreal arg)
{
    if (m_spacing == arg) return;
    m_spacing = arg;
    emit spacingChanged(arg);
}

void SpacedLocator::updateLocator()
{
    // Round the start value up to the next valid value
    QList<qreal> newLocs;
    newLocs << qCeil(start()/m_spacing)*m_spacing;
    while (newLocs.last() <= end())
        newLocs << newLocs.last() + m_spacing;
    // The last one is larger than the end value.
    newLocs.removeLast();
    setLocations(newLocs);
}

qreal SpacedLocator::spacing() const
{
    return m_spacing;
}
