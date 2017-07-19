#include "fluidsimulator.h"
#include <math.h>
#include <random>

FluidSimulator::FluidSimulator()
{
    m_grid= Grid(5,5,15,15);
    initCellCentres();
    initBoundaries();
    emitParticles(50,1,0.25f);
    //emitParticlesPerCell(8,0,1.0f);
}

FluidSimulator::~FluidSimulator(){}

void FluidSimulator::setFrameReady(const bool _frameReady)
{
    this->m_frameReady = _frameReady;
    if(this->m_event != nullptr)
    {
        this->m_event(_frameReady);
    }
}

void FluidSimulator::registerFrameReadyHandler(FrameReadyHandler _handler)
{
    this->m_event = _handler;
}


void FluidSimulator::initBoundaries()
{
    for(auto cell : m_grid.column(0))
    {
        cell->setLabel(Label::SOLID);
    }

    for(auto cell : m_grid.column(m_grid.nColumns()-1))
    {
        cell->setLabel(Label::SOLID);
    }

    for(auto cell : m_grid.row(0))
    {
        cell->setLabel(Label::SOLID);
    }

    for(auto cell : m_grid.row(m_grid.nRows()-1))
    {
        cell->setLabel(Label::SOLID);
    }

    m_simulationSize = 0;
    Grid::iterator it = m_grid.begin();
    while(it!=m_grid.end())
    {
        if(it->label()==Label::SOLID)
        {
            it++;
            continue;
        }
        m_simulationSize++;
        it++;
    }
}


//Emit _count particles for each cell
void FluidSimulator::emitParticlesPerCell(size_t _count,size_t _seed, float _velocity)
{
   std::default_random_engine generator(_seed);
   std::uniform_real_distribution<float> posUDistribution(0.0f,m_grid.deltaU());
   std::uniform_real_distribution<float> posVDistribution(0.0f,m_grid.deltaV());

   std::uniform_real_distribution<float> velDistribution(0.0f,_velocity);

   vec2 pos;
   vec2 velocity;

   Grid::iterator cell_it = m_grid.begin();
   while(cell_it != m_grid.end())
   {
       if(cell_it->label() == Label::SOLID)
       {
           cell_it++;
           continue;
       }

       for(size_t i = 0; i<_count; ++i)
       {
           //random particle initial position
           pos.m_x = posUDistribution(generator) + cell_it->minU();
           pos.m_y = posVDistribution(generator) + cell_it->minV();

           //random particle inital velocity
           velocity.m_x = velDistribution(generator);
           velocity.m_y = velDistribution(generator);

           //add particle to the particle pool
           m_particlePool.push_back(Particle(pos,velocity));

       }
       cell_it++;
   }

}

//Emit particles at random position all over the grid
//If a particle happens to be in a cell marked as solid, it won't be created
void FluidSimulator::emitParticles(size_t _count,size_t _seed,float _velocity)
{
   std::default_random_engine generator(_seed);
   std::uniform_real_distribution<float> posUDistribution(0.0f,m_grid.width());
   std::uniform_real_distribution<float> posVDistribution(0.0f,m_grid.height());

   std::uniform_real_distribution<float> velDistribution(0.0f,_velocity);

   vec2 pos;
   vec2 velocity;
   cell_ptr c;
   for(size_t i = 0; i<_count; ++i)
   {

       //random particle initial position
       pos.m_x = posUDistribution(generator);
       pos.m_y = posVDistribution(generator);

       //random particle inital velocity
       velocity.m_x = velDistribution(generator);
       velocity.m_y = velDistribution(generator);

       //retreive the cell at the current position
       c = &(m_grid.cell(pos));

       //if the cell is marked as solid, skip iteration
       if(c->label()== Label::SOLID)
       {
           continue;
       }

       //add particle to the particle pool
       m_particlePool.push_back(Particle(pos,velocity));

   }

}

void FluidSimulator::advanceFrame()
{
    float frameTime = 1.0f/30.0f; // 30Hz
    float CFLCoefficient = m_cfl; //TODO
    while(!m_frameReady)
    {
        if(frameTime <= 0.0f)
        {
            m_frameReady = true;
            break;
        }

        float simulationTimeStep;
        if(m_grid.maxVelocity()==0)
        {
            simulationTimeStep = frameTime;
        }
        else
        {
            simulationTimeStep = CFLCoefficient/m_grid.maxVelocity();
        }

        if(simulationTimeStep > frameTime)
        {
            simulationTimeStep=frameTime;
        }

        //Lower velocities require less iterations
        routineFLIP(simulationTimeStep);
        frameTime -= simulationTimeStep;
    }
    m_frameReady = false;
}

void FluidSimulator::routineFLIP(float _timeStep)
{
    transferToGrid();

    m_grid.deltaVelocityUpdate();

    advectParticles(_timeStep);

    m_grid.maxVelocityUpdate();
    markCells();
}

void FluidSimulator::transferToGrid()
{
    //Weight = totNumberOfParticles/ nonBoundaryCells
    float W = m_particlePool.size()/static_cast<float>(m_simulationSize);
    float k_temp = 0.0f;

    float temp_initialVelocity;

    //reset initial velocity U,V to 0.0f
    m_grid.resetInitialVelocity();

    Grid::iterator cell_it;
    for(auto &p: m_particlePool)
    {
        cell_it = m_grid.begin();
        while(cell_it != m_grid.end())
        {
            k_temp = kernel(p.m_position-cell_it->halfEdge('W'));
            temp_initialVelocity = cell_it->initialVelocityU();
            temp_initialVelocity += p.m_velocity.m_x*(k_temp/W);

            cell_it->setInitialVelocityU(temp_initialVelocity);

            k_temp = kernel(p.m_position-cell_it->halfEdge('S'));
            temp_initialVelocity = cell_it->initialVelocityV();
            temp_initialVelocity += p.m_velocity.m_y*(k_temp/W);

            cell_it->setInitialVelocityV(temp_initialVelocity);

            cell_it++;
        }
    }
}

float FluidSimulator::kernel(vec2 _xp)
{
    return h(_xp.m_x/m_grid.deltaU())
            *h(_xp.m_y/m_grid.deltaV());
}

float FluidSimulator::h(float _r)
{
    if(_r>=0||_r<=1)
        return 1-_r;
    if(_r>=-1 || _r<0)
        return 1+_r;
    return 0;
}

void FluidSimulator::advectParticles(float _timeStep)
{

    size_t tempIndex;
    //Forward Euler Advection
    for(auto &p : m_particlePool)
    {
        //interpolate the delta velocity from the grid and add to particle's velocity
        p.m_velocity += m_grid.deltaVelocity(p.m_position);

        //Update particle position
        p.m_position += _timeStep*p.m_velocity;
    }
}


void FluidSimulator::markCells()
{
    std::vector<Cell>::iterator cell_it = m_grid.begin();
    while(cell_it != m_grid.end())
    {
        if(cell_it->label()==Label::FLUID)
        {
            cell_it->setLabel(Label::EMPTY);
            cell_it->setStatus(Status::INACTIVE);
            cell_it->resetParticleCount();
        }
        cell_it++;
    }

    Cell* c;
    for(auto &p :m_particlePool)
    {
        c = &(m_grid.cell(p.m_position));
        c->setLabel(Label::FLUID);
        c->setStatus(Status::ACTIVE);
        c->incrementParticleCount();
    }
}








/********************************STUB METHOD!! SUBSTITUTE!!!******************************************************/
std::vector<FluidSimulator::vec3> FluidSimulator::velocityField(float _time)//doesn't take parameters
{
    std::vector<vec3> data;
    vec3 vertex;
    vertex.m_z = 0.0f;

    std::vector<vec3>::iterator centres_it = m_cellCentres.begin();
    while(centres_it != m_cellCentres.end())
    {
        vertex.m_x = centres_it->m_x + sinf(centres_it->m_x + _time);//sin di qualcosa
        vertex.m_y = centres_it->m_y + sinf(centres_it->m_y + _time) ;//cos di qualcosa
        data.push_back(vertex);
        centres_it++;
    }

    return data;
}
/**************************************************************************************/

/********************************STUB METHOD!! SUBSTITUTE!!!******************************************************/
std::vector<FluidSimulator::vec3> FluidSimulator::activeCells(float _time)
{
    std::vector<vec3> data;
/*
    std::default_random_engine generator(_time);
    std::uniform_int_distribution<int> distribution(0,m_cellCentres.size()-1);

    int activeCells = distribution(generator);

    for (int i=0; i<activeCells; ++i)
    {
        data.push_back(m_cellCentres[distribution(generator)]);

    }
    */

    vec3 centre;
    centre.m_z = 0.0f;
    Grid::iterator cell_it = m_grid.begin();
    while(cell_it != m_grid.end())
    {
        if(cell_it->status() == Status::INACTIVE)
        {
            cell_it++;
            continue;
        }

        centre.m_x = cell_it->centre().m_x;
        centre.m_y = cell_it->centre().m_y;
        data.push_back(centre);

        cell_it++;
    }
    return data;

}
/**************************************************************************************/

/********************************STUB METHOD!! SUBSTITUTE!!!******************************************************/
std::vector<FluidSimulator::vec3> FluidSimulator::particles()
{
    std::vector<vec3> data;
    vec3 pos;
    pos.m_z = 0;
    for(auto &p_pos: m_particlePool)
    {
        pos.m_x = p_pos.m_position.m_x;
        pos.m_y = p_pos.m_position.m_y;

        data.push_back(pos);
    }
    return data;
}
/**************************************************************************************/

void FluidSimulator::initCellCentres(){
    vec3 centre;
    centre.m_z = 0;

    float deltaU = width()/nColumns();
    float deltaV = height()/nRows();


    for(float y = deltaV/2; y<height();y+=deltaV)
    {
        for(float x = deltaU/2; x<width();x+=deltaU)
        {
            centre.m_x = x;
            centre.m_y = y;
            m_cellCentres.push_back(centre);
        }
    }
}

std::vector<FluidSimulator::vec3> FluidSimulator::boundaries()
{
    std::vector<vec3> data;

    vec3 centre;
    centre.m_z = 0.0f;
    Grid::iterator cell_it = m_grid.begin();
    while(cell_it != m_grid.end())
    {
        if(cell_it->label() == Label::SOLID)
        {
            centre.m_x = cell_it->centre().m_x;
            centre.m_y = cell_it->centre().m_y;
            data.push_back(centre);
        }
        cell_it++;
    }
    return data;

}
