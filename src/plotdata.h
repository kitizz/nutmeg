#ifndef PLOTDATA_H
#define PLOTDATA_H

#include <QAbstractTableModel>

class PlotData : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(QList<qreal> xData READ xData NOTIFY xDataChanged)
    Q_PROPERTY(QList<qreal> yData READ yData NOTIFY yDataChanged)

public:
    explicit PlotData(QObject *parent = 0);

    enum Role{
        X = Qt::UserRole,
        Y
    };

    // Reimplement virtual methods
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex& index, int role=Qt::DisplayRole) const;


    QList<qreal> xData() const;
    QList<qreal> yData() const;
    Q_INVOKABLE void setPlotData(QList<qreal> xData, QList<qreal> yData);
//    void setYData(QList<qreal> arg);

signals:
    void xDataChanged(QList<qreal> arg);
    void yDataChanged(QList<qreal> arg);

public slots:

private:
    QList<qreal> m_xData;
    QList<qreal> m_yData;
};

#endif // PLOTDATA_H
