#ifndef PLOTCANVAS_H
#define PLOTCANVAS_H

#include "../../nutmeglibdecl.h"
//#include <QQuickPaintedItem>
#include <QQuickFramebufferObject>
#include "../../common/plotbase.h"

class NUTMEGLIB_EXPORT PlotCanvas : public QQuickFramebufferObject::Renderer
{
public:
    PlotCanvas(PlotBase *parent = 0);

    qreal scaling() const;
    void setScaling(qreal arg);

    PlotBase *parent() const;
    void dataChanged();

protected:
    PlotBase *m_parent;
    bool m_dirtyData;

private:
    qreal m_scaling;
    bool m_updateTriggered;
};

#endif // PLOTCANVAS_H
