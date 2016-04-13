#include "layoutcolumn.h"

LayoutColumn::LayoutColumn(QQuickItem *parent)
    : LayoutGrid(parent)
{
}

LayoutColumn::~LayoutColumn()
{

}

int LayoutColumn::columns() const
{
    return 1;
}

LayoutGrid::LayoutDirection LayoutColumn::layoutDirection() const
{
    return LayoutGrid::Vertical;
}

