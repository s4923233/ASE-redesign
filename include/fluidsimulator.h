#ifndef FLUIDSIMULATOR_H
#define FLUIDSIMULATOR_H
#include <ngl/Vec3.h>
#include <vector>
#include <queue>

#include "grid.h"

#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Sparse>
#include <eigen3/Eigen/IterativeLinearSolvers>
using Eigen::VectorXd;
using Eigen::VectorXi;
using Eigen::SparseMatrix;
using Eigen::Triplet;
using Eigen::RowMajor;
using Eigen::ConjugateGradient;
using Eigen::Success;
typedef Triplet<double> Tripletd;

enum class Colour{WHITE,GRAY,BLACK};

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

    void setPressureSolverMode(bool _mode)           {m_pressureSolverMode = _mode;}

    std::vector<vec3> velocityField(float _time);
    std::vector<vec3> activeCells(float _time);
    std::vector<vec3> boundaries();

    void boundaryCollide(particle_ptr _p);

    std::vector<vec3> particles();

    void advanceFrame();
    void routineFLIP(float _timeStep);
    void transferToGrid();
    void advectParticles(float _timeStep);
    void markCells();
    vec2 particleTrace(vec2 _pos, float _timeStep);
    void advectVelocity(float _timeStep);
    VectorXd negativeDivergence(float _timeStep);
    SparseMatrix<double,RowMajor> setUpMatrixA(float _timeStep);
    void updatePressureField(VectorXd _p);
    void pressureGradientUpdate(float _timeStep);
    void pressureSolve(float _timeStep);


protected:
    void setFrameReady(const bool _frameReady);
//    void registerFrameReadyHandler(FrameReadyHandler _handler);

    void initCellCentres();
    void initBoundaries();
    void emitParticlesPerCell(size_t _count, size_t _seed = 0, float _velocity = 0.0f);
    void emitParticles(size_t _count,size_t _seed = 0,float _velocity=0.0f);

    float kernel(vec2 _xp);
    float h(float _r);

    float usolid(size_t _x, size_t _y);
    float vsolid(size_t _x, size_t _y);

private:

    bool m_frameReady = false;
    bool m_pressureSolverMode = false;
    float m_cfl = 2.0;


    Grid m_grid;
    std::vector<vec3> m_cellCentres;
    std::vector<Particle> m_particlePool;
    size_t m_simulationSize;
};

#endif // FLUIDSIMULATOR_H
