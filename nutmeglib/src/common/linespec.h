#ifndef LINESPEC_H
#define LINESPEC_H

#include "../nutmeglibdecl.h"
#include <QObject>
#include <QColor>
#include <QMap>

#include "nutmegobject.h"

class NUTMEGLIB_EXPORT LineSpec : public QObject, public NutmegObject
{
    Q_OBJECT
    Q_PROPERTY(QString spec READ spec WRITE setSpec NOTIFY specChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QString style READ style WRITE setStyle NOTIFY styleChanged)
    Q_PROPERTY(qreal width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(QString marker READ marker WRITE setMarker NOTIFY markerChanged)

public:
    explicit LineSpec(QObject* parent=0);
    explicit LineSpec(QString spec, QObject *parent=0);
    QStringList toList() const;
    QString toString() const;

    static QString markers() { return "+o*.xsd^v><ph"; } // TODO: Put in MarkerSpec
    static QMap<QString, QColor> colorMap;
    static QMap<QString, Qt::PenStyle> styleMap;

    QString spec() const;
    void setSpec(QString arg);

    QColor color() const;
    void setColor(QColor arg);

    QString style() const;
    void setStyle(QString arg);

    qreal width() const;
    void setWidth(qreal arg);

    QString marker() const;
    void setMarker(QString arg);

signals:
    void specChanged(QString arg);
    void colorChanged(QColor arg);
    void styleChanged(QString arg);
    void widthChanged(qreal arg);
    void markerChanged(QString arg);

private:
//    QString m_spec;
    QColor m_color;
    QString m_colorStr;
    QString m_style;
    qreal m_width;
    QString m_marker;

    static QMap<QString, QColor> generateColorMap() {
        QMap<QString, QColor> mapp = QMap<QString, QColor>();
        mapp.insert("r", Qt::red);
        mapp.insert("g", Qt::green);
        mapp.insert("b", Qt::blue);
        mapp.insert("c", Qt::cyan);
        mapp.insert("m", Qt::magenta);
        mapp.insert("y", Qt::yellow);
        mapp.insert("k", Qt::black);
        mapp.insert("w", Qt::white);
        return mapp;
    }

    static QMap<QString, Qt::PenStyle> generateStyleMap() {
        QMap<QString, Qt::PenStyle> mapp = QMap<QString, Qt::PenStyle>();
        mapp.insert("-", Qt::SolidLine);
        mapp.insert("--", Qt::DashLine);
        mapp.insert(":", Qt::DotLine);
        mapp.insert("-.", Qt::DashDotLine);
        return mapp;
    }
};

#endif // LINESPEC_H
