#ifndef NUTMEGRENDERER_H
#define NUTMEGRENDERER_H

#include <QOpenGLFunctions_3_3_Core>

// TODO: Insert NUTMEGLIB_EXPORT??
class NutmegRenderer : public QOpenGLFunctions_3_3_Core
{
public:
    NutmegRenderer()
        : QOpenGLFunctions_3_3_Core()
        , m_dirtyData(true)
    {
    }

    virtual void initialize() = 0;
    virtual void render(QRectF view, QSize surfaceSize) = 0;

    void dataChanged() {
        m_dirtyData = true;
    }

protected:
    bool m_dirtyData;
};

#endif // NUTMEGRENDERER_H
