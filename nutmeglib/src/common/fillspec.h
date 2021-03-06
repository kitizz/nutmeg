#ifndef FILLSPEC_H
#define FILLSPEC_H

#include <QObject>
#include <QColor>
#include "nutmegobject.h"

class NUTMEGLIB_EXPORT FillSpec : public QObject, public NutmegObject
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
//    Q_PROPERTY(qreal alpha READ alpha WRITE setAlpha NOTIFY alphaChanged)

public:
    explicit FillSpec(QObject *parent = 0);

    QColor color() const;
    void setColor(QColor arg);

    qreal alpha() const;
    void setAlpha(qreal arg);

signals:
    void colorChanged(QColor arg);
    void alphaChanged(qreal arg);

public slots:

private:
    QColor m_color;
    qreal m_alpha;

};

#endif // FILLSPEC_H
