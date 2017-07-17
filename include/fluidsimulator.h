#ifndef FLUIDSIMULATOR_H
#define FLUIDSIMULATOR_H
#include <ngl/Vec3.h>
#include <vector>

#include "grid.h"

typedef void (*FrameReadyHandler)(bool _newFrame);

class FluidSimulator
{
    typedef ngl::Vec3 vec3;

public:
    FluidSimulator();
    ~FluidSimulator();

    float width() const              {return m_grid.width();}
    float height() const              {return m_grid.height();}
    size_t nColumns() const              {return m_grid.nColumns();}
    size_t nRows()const              {return m_grid.nRows();}
    std::vector<vec3> cellCentres()const             {return m_cellCentres;}

    std::vector<vec3> velocityField(float _time);
    std::vector<vec3> activeCells(float _time);
    std::vector<vec3> particles();

protected:
    void setFrameReady(const bool _frameReady);
    void registerFrameReadyHandler(FrameReadyHandler _handler);

    void initCellCentres();

private:

    bool m_frameReady = false;
    FrameReadyHandler m_event = nullptr;

    Grid m_grid;
    std::vector<vec3> m_cellCentres;
    std::vector<vec3> m_particlePool;//to substitute vec3 with Particle
};

#endif // FLUIDSIMULATOR_H
