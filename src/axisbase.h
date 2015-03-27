#ifndef AXIS2DBASE_H
#define AXIS2DBASE_H

#include <QQuickItem>
#include "nutmegobject.h"
#include "figurebase.h"

class FigureBase;
class AxisBase : public QQuickItem, public NutmegObject
{
    Q_OBJECT
    Q_PROPERTY(QString handle READ handle WRITE setHandle NOTIFY handleChanged)
    Q_PROPERTY(FigureBase* figure READ figure WRITE setFigure NOTIFY figureChanged)

    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QFont titleFont READ titleFont WRITE setTitleFont NOTIFY titleFontChanged)
    Q_PROPERTY(QColor titleColor READ titleColor WRITE setTitleColor NOTIFY titleColorChanged)
public:
    explicit AxisBase(QQuickItem *parent = 0);

    // NutmegObject interface
    Q_INVOKABLE void registerProperties(QMap<QString, QString> mapping);
    Q_INVOKABLE void registerProperties(QVariantMap mapping);
    Q_INVOKABLE QString mapProperty(const QString &prop);

    QVariantMap plots(); // For QML
    PlotBase *getPlotByHandle(QString handle); // For C++
    Q_INVOKABLE QVariantList getPlotList();

    QString handle() const;
    void setHandle(QString arg);

    FigureBase* figure() const;
    void setFigure(FigureBase* arg);

    QString title() const;
    void setTitle(QString arg);

    QFont titleFont() const;
    void setTitleFont(QFont arg);

    QColor titleColor() const;
    void setTitleColor(QColor arg);

signals:
    void handleChanged(QString arg);
    void figureChanged(FigureBase* arg);
    void registerWithFigure(Axis2DBase* axis);
    void titleChanged(QString arg);
    void titleFontChanged(QFont arg);
    void titleColorChanged(QColor arg);

public slots:
    void updateFigure();

private:
    QString m_handle;
    FigureBase* m_figure;
    QString m_title;
    QFont m_titleFont;
    QColor m_titleColor;
};

#endif // AXIS2DBASE_H
