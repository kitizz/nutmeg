#ifndef LAYOUTROW_H
#define LAYOUTROW_H

#include "layoutgrid.h"

class NUTMEGLIB_EXPORT LayoutRow : public LayoutGrid
{
    Q_OBJECT
    Q_PROPERTY(int rows READ rows NOTIFY rowsChanged)
    Q_PROPERTY(LayoutGrid::LayoutDirection layoutDirection READ layoutDirection NOTIFY layoutDirectionChanged)

public:
    LayoutRow(QQuickItem *parent=0);
    ~LayoutRow();

    int rows() const;
    LayoutGrid::LayoutDirection layoutDirection() const;

signals:
    void rowsChanged(int arg);
    void layoutDirectionChanged(LayoutGrid::LayoutDirection arg);
};

#endif // LAYOUTROW_H
