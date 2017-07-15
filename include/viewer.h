#ifndef VIEWER_H
#define VIEWER_H
#include <QOpenGLWidget>

#include "FluidSimulator.h"

class Viewer : public QOpenGLWidget
{
public:
    Viewer(const FluidSimulator &_fluidSimulator);
    ~Viewer();

    void setModel(const FluidSimulator& _fluidSimulator);
    void render();
private:
    FluidSimulator m_fluidSimulator;
};

#endif // VIEWER_H
