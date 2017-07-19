#ifndef FLUIDSIMULATOR_H
#define FLUIDSIMULATOR_H
#include <ngl/Vec3.h>
#include <vector>

#include "grid.h"

typedef void (*FrameReadyHandler)(bool _newFrame);

class FluidSimulator
{
    typedef ngl::Vec3 vec3;
    typedef ngl::Vec2 vec2;
    typedef Particle* particle_ptr;
    typedef Cell* cell_ptr;

public:
    FluidSimulator();
    ~FluidSimulator();

    float width() const                              {return m_grid.width();}
    float height() const                             {return m_grid.height();}
    size_t nColumns() const                          {return m_grid.nColumns();}
    size_t nRows()const                              {return m_grid.nRows();}
    size_t simulationSize() const                    {return m_simulationSize;}
    std::vector<vec3> cellCentres()const             {return m_cellCentres;}

    std::vector<vec3> velocityField(float _time);
    std::vector<vec3> activeCells(float _time);
    std::vector<FluidSimulator::vec3> boundaries();

    std::vector<vec3> particles();

    void advanceFrame();
    void routineFLIP(float _timeStep);
    void transferToGrid();
    void advectParticles(float _timeStep);
    void markCells();


protected:
    void setFrameReady(const bool _frameReady);
    void registerFrameReadyHandler(FrameReadyHandler _handler);

    void initCellCentres();
    void initBoundaries();
    void emitParticlesPerCell(size_t _count, size_t _seed = 0, float _velocity = 0.0f);
    void emitParticles(size_t _count,size_t _seed = 0,float _velocity=0.0f);

    float kernel(vec2 _xp);
    float h(float _r);
private:

    bool m_frameReady = false;
    float m_cfl = 2.0;
    FrameReadyHandler m_event = nullptr;

    Grid m_grid;
    std::vector<vec3> m_cellCentres;
    std::vector<Particle> m_particlePool;
    size_t m_simulationSize;
    //std::vector<vec3> m_particlePool;//to substitute vec3 with Particle
};

#endif // FLUIDSIMULATOR_H
