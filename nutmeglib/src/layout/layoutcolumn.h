#ifndef LAYOUTCOLUMN_H
#define LAYOUTCOLUMN_H

#include "layoutgrid.h"


class NUTMEGLIB_EXPORT LayoutColumn : public LayoutGrid
{
    Q_OBJECT
    Q_PROPERTY(int columns READ columns NOTIFY columnsChanged)
    Q_PROPERTY(LayoutGrid::LayoutDirection layoutDirection READ layoutDirection NOTIFY layoutDirectionChanged)

public:
    LayoutColumn(QQuickItem *parent=0);
    ~LayoutColumn();

    int columns() const;
    LayoutGrid::LayoutDirection layoutDirection() const;

signals:
    void columnsChanged(int arg);
    void layoutDirectionChanged(LayoutGrid::LayoutDirection arg);
};

#endif // LAYOUTCOLUMN_H
