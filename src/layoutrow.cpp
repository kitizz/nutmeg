#include "layoutrow.h"

LayoutRow::LayoutRow(QQuickItem *parent)
    : LayoutGrid(parent)
{
    setLayoutDirection(Vertical);
}

LayoutRow::~LayoutRow()
{

}

int LayoutRow::rows() const
{
    return 1;
}

LayoutGrid::LayoutDirection LayoutRow::layoutDirection() const
{
    return LayoutGrid::Horizontal;
}

