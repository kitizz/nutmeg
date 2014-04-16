#include "plotdata.h"
#include <QDebug>

PlotData::PlotData(QObject *parent) :
    QAbstractTableModel(parent)
{
}

int PlotData::columnCount(const QModelIndex &) const
{
    return m_xData.length();
}

int PlotData::rowCount(const QModelIndex &) const
{
    return 2;
}

QVariant PlotData::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole) return QVariant();

    // The data should be 2xN in size
    switch (index.row()) {
    case 0:
        return m_xData[index.column()];
    case 1:
        return m_yData[index.column()];
    default:
        return QVariant();
    }
}

QList<qreal> PlotData::xData() const
{
    return m_xData;
}

QList<qreal> PlotData::yData() const
{
    return m_yData;
}

void PlotData::setPlotData(QList<qreal> xData, QList<qreal> yData)
{
    if (m_xData == xData && m_yData == yData) return;
    if (xData.length() != yData.length()) {
        qWarning() << "X and Y data are not the same length - not updated. "
                    << "X: " << xData.length()
                    << ", Y: " << yData.length();
        return;
    }
    m_xData = xData;
    m_yData = yData;
    qDebug() << "Data Changed";
    emit dataChanged(index(0,0), index(1, m_xData.length()-1));
}
