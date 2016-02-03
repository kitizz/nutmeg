#ifndef LAYOUTGRID_H
#define LAYOUTGRID_H

#include <QQuickItem>
#include "axisbase.h"

class AxisBase;
class LayoutGrid : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QString handle READ handle WRITE setHandle NOTIFY handleChanged)
    Q_PROPERTY(int rows READ rows WRITE setRows NOTIFY rowsChanged)
    Q_PROPERTY(int columns READ columns WRITE setColumns NOTIFY columnsChanged)
    Q_PROPERTY(LayoutDirection layoutDirection READ layoutDirection WRITE setLayoutDirection NOTIFY layoutDirectionChanged)
    Q_PROPERTY(bool reversed READ reversed WRITE setReversed NOTIFY reversedChanged)
    Q_PROPERTY(qreal vspace READ vspace WRITE setVspace NOTIFY vspaceChanged)
    Q_PROPERTY(qreal hspace READ hspace WRITE setHspace NOTIFY hspaceChanged)

    Q_PROPERTY(QList<qreal> rowWeights READ rowWeights WRITE setRowWeights NOTIFY rowWeightsChanged)
//    Q_PROPERTY(QList<qreal> rowMinimums READ rowMinimums WRITE setRowMinimums NOTIFY rowMinimumsChanged)
//    Q_PROPERTY(QList<qreal> rowMaximums READ rowMaximums WRITE setRowMaximums NOTIFY rowMaximumsChanged)

    Q_PROPERTY(QList<qreal> columnWeights READ columnWeights WRITE setColumnWeights NOTIFY columnWeightsChanged)
//    Q_PROPERTY(QList<qreal> columnMinimums READ columnMinimums WRITE setColumnMinimums NOTIFY columnMinimumsChanged)
//    Q_PROPERTY(QList<qreal> columnMaximums READ columnMaximums WRITE setColumnMaximums NOTIFY columnMaximumsChanged)
    Q_PROPERTY(QList<AxisBase*> axes READ axes NOTIFY axesChanged)
//    Q_PROPERTY(QQmlListProperty<AxisBase> axes READ axes NOTIFY axesChanged)
//    Q_CLASSINFO("DefaultProperty", "axes")
    Q_ENUMS(LayoutDirection)

public:
    LayoutGrid(QQuickItem *parent = 0);
    ~LayoutGrid();

    enum LayoutDirection { Horizontal=0, Vertical=1 };

    QString handle() const;
    void setHandle(QString arg);

    int rows() const;
    void setRows(int arg);
    int columns() const;
    void setColumns(int arg);
    // Convenience function for C++
    void getSize(int &rows, int &columns);

    QList<AxisBase*> axes() const;
//    QQmlListProperty<AxisBase> axes();
//    static void appendAxis(QQmlListProperty<AxisBase> *list, AxisBase* ax);
//    static void axesClear(QQmlListProperty<AxisBase> *list);
//    static int axisCount(QQmlListProperty<AxisBase> *list);
//    static AxisBase *axis(QQmlListProperty<AxisBase> *list, int index);

    AxisBase *axisAt(const int row, const int col);

//    static AxisLayoutAttached *qmlAttachedProperties(QObject *);

    LayoutDirection layoutDirection() const;
    void setLayoutDirection(LayoutDirection arg);

    bool reversed() const;
    void setReversed(bool arg);

    QList<qreal> rowWeights() const;
    void setRowWeights(QList<qreal> arg);
//    QList<qreal> rowMinimums() const;
//    void setRowMinimums(QList<qreal> arg);
//    QList<qreal> rowMaximums() const;
//    void setRowMaximums(QList<qreal> arg);

    QList<qreal> columnWeights() const;
    void setColumnWeights(QList<qreal> arg);
//    QList<qreal> columnMinimums() const;
//    void setColumnMinimums(QList<qreal> arg);
//    QList<qreal> columnMaximums() const;
//    void setColumnMaximums(QList<qreal> arg);

    void setPlotRectsDirty();
    QRectF getPlotRectFor(const int row, const int col);

    qreal vspace() const;
    void setVspace(qreal vspace);
    qreal hspace() const;
    void setHspace(qreal hspace);

signals:
    void handleChanged(QString arg);
    void rowsChanged(int arg);
    void columnsChanged(int arg);
    void axesChanged();

    void layoutDirectionChanged(LayoutDirection arg);
    void reversedChanged(bool arg);

    void rowWeightsChanged(QList<qreal> arg);
//    void rowMinimumsChanged(QList<qreal> arg);
//    void rowMaximumsChanged(QList<qreal> arg);

    void columnWeightsChanged(QList<qreal> arg);
//    void columnMinimumsChanged(QList<qreal> arg);
//    void columnMaximumsChanged(QList<qreal> arg);

    void vspaceChanged(qreal vspace);
    void hspaceChanged(qreal hspace);

public slots:
    void updateAxes();
    virtual void updateLayout();
    void updatePlotRects();

protected:
    QList<AxisBase*> m_axes;

private:
    QVector<qreal> cumSum(QVector<qreal> values);
    QVector<qreal> distributeWeights(const int N, const QList<qreal> weights, const qreal size);
    inline int plotIndex(const int row, const int col) {
        int index = m_layoutDirection == Horizontal ?
                    row*m_columns + col : index = col*m_rows + row;
        if (m_reversed)
            index = m_axes.count() - index;
        return index;
    }

//    FigureBase *getFigure();
    QString m_handle;
    int m_rows;
    int m_columns;
    LayoutDirection m_layoutDirection;
    bool m_reversed;
    qreal m_vspace;
    qreal m_hspace;
    QList<qreal> m_rowWeights, m_columnWeights;
//    QList<qreal> m_rowMinimums;
//    QList<qreal> m_rowMaximums;
//    QList<qreal> m_columnWeights;
//    QList<qreal> m_columnMinimums;
//    QList<qreal> m_columnMaximums;
    QVector<qreal> m_minXs, m_maxXs, m_minYs, m_maxYs;
    bool m_plotRectsDirty;
};

//QML_DECLARE_TYPEINFO(LayoutGrid, QML_HAS_ATTACHED_PROPERTIES)

#endif // LAYOUTGRID_H
