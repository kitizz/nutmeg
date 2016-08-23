#ifndef AXIS2DRENDERER_H
#define AXIS2DRENDERER_H

#include "../../common/nutmegrenderer.h"

/**
 * @brief The AxisRenderer class handles the drawing of axis ticks, text, grids, etc.
 */
class Axis2DRenderer : public NutmegRenderer
{
public:
    Axis2DRenderer();

    virtual void initialize();
    virtual void render(QRectF view, QSize surfaceSize);
};

#endif // AXIS2DRENDERER_H
