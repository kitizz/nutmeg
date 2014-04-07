#ifndef PLOTCANVAS_H
#define PLOTCANVAS_H

#include <QQuickPaintedItem>

class PlotCanvas : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(qreal scaling READ scaling WRITE setScaling NOTIFY scalingChanged)
    Q_PROPERTY(QObject* app READ app WRITE setApp NOTIFY appChanged)

public:
    explicit PlotCanvas(QQuickItem *parent = 0);

    qreal scaling() const;
    void setScaling(qreal arg);

    QObject* app() const;
    void setApp(QObject* arg);

    bool eventFilter(QObject *watched, QEvent *event);

signals:
    void scalingChanged(qreal arg);

    void appChanged(QObject* arg);

public slots:

private:
    qreal m_scaling;

    QObject* m_app;
};

#endif // PLOTCANVAS_H
