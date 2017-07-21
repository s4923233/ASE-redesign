#include "fluidsimulator.h"
#include <math.h>
#include <cmath>
#include <random>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
/// @file cell.cpp
/// @brief implementation files for Cell class
//----------------------------------------------------------------------------------------------------------------------
FluidSimulator::FluidSimulator()
{
    //create a grid 15 columns 15 rows
    m_grid = Grid(5,5,15,15);

    //finds the cell centres to simplify the visualization data
    initCellCentres();

    //marks cells at simulation edges as solid
    initBoundaries();

    //emits at most 20 particles, randomly distributed.
    emitParticles(500,1,0.25f);

    //uncomment to emit 5 particles per cell
    //emitParticlesPerCell(5,0,1.0f);

    size_t nParticles = m_particlePool.size();

    //tells the cell how many particles have been initialised
    Grid::iterator cell_it = m_grid.begin();
    while(cell_it!=m_grid.end())
    {
        cell_it->setParticlePoolSize(nParticles);
        cell_it++;
    }

    markCells();

}

//----------------------------------------------------------------------------------------------------------------------
FluidSimulator::~FluidSimulator(){}

void FluidSimulator::initBoundaries()
{
    //Takes the edges and sets them to Solid

    //grid.column(i) returns the i-th column in the grid
    for(auto cell : m_grid.column(0))
    {
        cell->setLabel(Label::SOLID);
    }

    for(auto cell : m_grid.column(m_grid.nColumns()-1))
    {
        cell->setLabel(Label::SOLID);
    }

    //grid.row(i) returns the i-th row in the grid
    for(auto cell : m_grid.row(0))
    {
        cell->setLabel(Label::SOLID);
    }

    for(auto cell : m_grid.row(m_grid.nRows()-1))
    {
        cell->setLabel(Label::SOLID);
    }

    //counts the number of non solid cells
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

//----------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------
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

        float simulationTimeStep = 0.0f;
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

//----------------------------------------------------------------------------------------------------------------------
//------------------------MAIN FLIP ROUTINE-----------------------------------------------------------------------------
void FluidSimulator::routineFLIP(float _timeStep)
{

    //transfer particle initial velocities to the grid and store the values on the grid.
    transferToGrid();

    //Compute the other terms on the grid like pressure projection to get an updated velocity
    if(m_pressureSolverMode)
    {
        pressureSolve(_timeStep);
    }

    //calculated the change as deltaVelocity = initialVelocity - updatedVelocity
    m_grid.deltaVelocityUpdate();

    //advect the particles in the grid velocity field
    advectParticles(_timeStep);

    //Updates th maximum velocity in the system
    m_grid.maxVelocityUpdate();

    //tracks the active cells
    markCells();
}

//----------------------------------------------------------------------------------------------------------------------
//Implements the transfer of velocity from particles to the grid
//It sums at each grid point the particles values modualted by a kernel function that gives
//the weights of the nearby particles
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
            //take only the values of nearby particles
            k_temp = kernel(p.m_position-cell_it->halfEdge('W'));
            temp_initialVelocity = cell_it->initialVelocityU();
            temp_initialVelocity += p.m_velocity.m_x*(k_temp/W);

            //stores the particle initial velocity in the U direction
            cell_it->setInitialVelocityU(temp_initialVelocity);

            //take only the values of nearby particles
            k_temp = kernel(p.m_position-cell_it->halfEdge('S'));
            temp_initialVelocity = cell_it->initialVelocityV();
            temp_initialVelocity += p.m_velocity.m_y*(k_temp/W);

            //stores the particle initial velocity in the U direction
            cell_it->setInitialVelocityV(temp_initialVelocity);

            cell_it++;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
//gives the weights of the nearby particles
float FluidSimulator::kernel(vec2 _xp)
{
    return h(_xp.m_x/m_grid.deltaU())
            *h(_xp.m_y/m_grid.deltaV());
}

//----------------------------------------------------------------------------------------------------------------------
//simple hat function
float FluidSimulator::h(float _r)
{
    if(_r>=0 && _r<=1)
        return 1-_r;
    if(_r>=-1 && _r<0)
        return 1+_r;
    return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void FluidSimulator::advectParticles(float _timeStep)
{
    //Forward Euler Advection
    for(auto &p : m_particlePool)
    {
        //interpolate the delta velocity from the grid and add to particle's velocity
        p.m_velocity += m_grid.deltaVelocity(p.m_position);

        //Update particle position
        p.m_position += _timeStep*p.m_velocity;
    }
}

//----------------------------------------------------------------------------------------------------------------------
void FluidSimulator::markCells()
{
    //reset the cell values (only the FLUID cells)
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

    //marks the cells containing particles as FLUID and ACTIVE
    Cell* c;
    for(auto &p :m_particlePool)
    {
        c = &(m_grid.cell(p.m_position));
        if(c->label()==Label::EMPTY)
        {
            c->setLabel(Label::FLUID);
            c->setStatus(Status::ACTIVE);
            c->incrementParticleCount();
        }
        else
        {
            //enforce boundary conditions
            boundaryCollide(&p);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
//basic solid velocity at the boundaries.
//If a particle travels outside the boundaries it is reprojected in the opposite normal direction
void FluidSimulator::boundaryCollide(particle_ptr _p)
{
    vec2 pos = _p->m_position;
    if(pos.m_x<=1 || pos.m_x>=m_grid.width()-1)
        _p->m_velocity.m_x *= -1;
    if(pos.m_y<=1 || pos.m_y>=m_grid.height()-1)
        _p->m_velocity.m_y *= -1;
}

//----------------------------------------------------------------------------------------------------------------------
//tracks an hypotetica particle along the velocity field, one step back in time
FluidSimulator::vec2 FluidSimulator::particleTrace(vec2 _pos, float _timeStep)
{
    vec2 velocity = m_grid.velocity(_pos);

    //forward Euler
    _pos -= _timeStep*velocity;

    //clamp position to boundaries
    _pos.m_x  = _pos.m_x < 0 ? 0.0f : _pos.m_x;
    _pos.m_x  = _pos.m_x > m_grid.width() ?  m_grid.width() : _pos.m_x;

    _pos.m_y = _pos.m_y < 0 ? 0.0f : _pos.m_y;
    _pos.m_y = _pos.m_y > m_grid.height() ? m_grid.height() : _pos.m_y;

    return _pos;
}

//----------------------------------------------------------------------------------------------------------------------
//Velocity field advection
void FluidSimulator::advectVelocity(float _timeStep)
{
    vec2 pos;

    std::vector<float> tempVelocityU;
    std::vector<float> tempVelocityV;

    //set the new velocity values equal to the old value of the hypotetical particle
    //that happen to be at that grid point.
    //Uses particle trace to track the velocit value

    //Initially it doesn't store the new velocity in the grid
    Grid::iterator cell_it = m_grid.begin();
    while(cell_it != m_grid.end())
    {
        //U velocity
        pos = particleTrace(cell_it->halfEdge('W'),_timeStep);
        tempVelocityU.push_back(m_grid.velocity(pos).m_x);

        //V velocity
        pos = particleTrace(cell_it->halfEdge('S'),_timeStep);
        tempVelocityV.push_back(m_grid.velocity(pos).m_y);
        cell_it++;
    }

    //Then, new velocities are then stored in the grid
    for(size_t i = 0; i<m_grid.size(); ++i)
    {
        m_grid.cell(i).setVelocityU(tempVelocityU[i]);
        m_grid.cell(i).setVelocityV(tempVelocityV[i]);
    }
}

//----------------------------------------------------------------------------------------------------------------------
//calculates the negative divergence which will become the right hand side of the linear sistem
//used to solve for the pressure
VectorXd FluidSimulator::negativeDivergence(float _timeStep)
{
    size_t height = nColumns()-1;
    size_t width = nRows()-1;
    size_t dim = m_grid.size();

    VectorXd b(dim);

    float dx = m_grid.deltaU();
    float scale = 1.0f/dx;

    vec2 coordinate;
    for(size_t i = 0; i<m_grid.size();++i)
    {
        b(i) = 0.0f;
        if(m_grid.cell(i).label() == Label::FLUID)
        {
            coordinate = m_grid.toCartesian(i);
            //takes the velocity divergence at the specified position
            b(i) = -scale*m_grid.velocityDivergence(coordinate.m_x,coordinate.m_y);
        }
    }

    //modify the right hand side b to take into account the velocit at the boundaries
    size_t index;
    for (size_t y = 0; y < height; ++y)
    {
        for (size_t x = 0; x < width; ++x)
        {
            index = m_grid.toIndex(x,y);
            if(m_grid.cell(x,y).label() == Label::FLUID)
            {
                //case the left cell is solid
                if(x>=1 && m_grid.cell(x-1,y).label() == Label::SOLID)
                {
                    //updates the b vector to take the boundaiìries into account
                    b(index) -= scale * (m_grid.cell(x,y).velocityU() - usolid(x,y));
                }

                //case the right cell is solid
                if((x+1)<m_grid.nColumns() && m_grid.cell(x+1,y).label() == Label::SOLID)
                {
                    b(index) += scale * (m_grid.cell(x+1,y).velocityU() - usolid(x+1,y));
                }

                //case the cell below is solid
                if(y>=1 && m_grid.cell(x,y-1).label() == Label::SOLID)
                {
                    b(index) -= scale * (m_grid.cell(x,y).velocityV()-vsolid(x,y));
                }

                //case the cell above is solid
                if((y+1)<m_grid.nColumns() && m_grid.cell(x,y+1).label() == Label::SOLID)
                {
                    b(index) += scale * (m_grid.cell(x,y+1).velocityV()-vsolid(x,y+1));
                }
            }
        }
    }

    //return the right hand side of the linear system
    return b;
}

//----------------------------------------------------------------------------------------------------------------------
//setup matrix entries for the pressure equation
SparseMatrix<double,RowMajor> FluidSimulator::setUpMatrixA(float _timeStep)
{

    size_t height = nColumns()-1;
    size_t width = nRows()-1;
    size_t dim = m_grid.size();

    //Step2. Set the entries of A
    SparseMatrix<double,RowMajor> A(dim,dim);

    float dx = m_grid.deltaU();
    float scale;

    for(size_t y=0;y<height;++y)
    {
        for(size_t x=0;x<width;++x)
        {
            scale = _timeStep/(m_grid.density(x,y)*dx*dx);
            size_t i = y*width+x;//row index of A
            size_t j = 0; //pressure equation coefficient index. Column index of A

            if(m_grid.cell(x,y).label() == Label::FLUID)
            {
                //U
                if(x>=1 && m_grid.cell(x-1,y).label() == Label::FLUID)
                {
                    //Adiag +=scale
                    j=y*width+x;
                    A.coeffRef(i,j) += scale;
                }
                if(x+1<width && m_grid.cell(x+1,y).label() == Label::FLUID)
                {
                    //Adiag +=scale
                    j=y*width+x;
                    A.coeffRef(i,j) += scale;
                    //Ax = -scale
                    j=(y+1)*width+x;
                    A.coeffRef(i,j) = -scale;
                }
                else if(x+1<width && m_grid.cell(x+1,y).label() == Label::EMPTY)
                {
                    //Adiag +=scale
                    j=y*width+x;
                    A.coeffRef(i,j) += scale;
                }
                //V
                if(y>=1 && m_grid.cell(x,y-1).label() == Label::FLUID)
                {
                    //Adiag +=scale
                    j=y*width+x;
                    A.coeffRef(i,j) += scale;
                }
                if(y+1<height && m_grid.cell(x,y+1).label() == Label::FLUID)
                {
                    //Adiag +=scale
                    j=y*width+x;
                    A.coeffRef(i,j) += scale;
                    //Ay = -scale
                    j=y*width+x+1;
                    A.coeffRef(i,j) = -scale;
                }
                else if(y+1<height && m_grid.cell(x,y+1).label() == Label::EMPTY)
                {
                    //Adiag +=scale
                    j=y*width+x;
                    A.coeffRef(i,j) += scale;
                }
            }
        }
    }
    return A;

}

//----------------------------------------------------------------------------------------------------------------------
//updates the grid pressure values
void FluidSimulator::updatePressureField(VectorXd _p)
{
    for(size_t i= 0;i<m_grid.size(); i++)
    {
        m_grid.pressure(i,_p(i));
    }
}

//----------------------------------------------------------------------------------------------------------------------
//pressure gradient update
//calculates the new velocities taking the pressure values into account
void FluidSimulator::pressureGradientUpdate(float _timeStep)
{
    size_t height = nColumns()-1;
    size_t width = nRows()-1;

    float scale = 0.0f;
    float dx = m_grid.deltaU();
    float pressure_dx = 0.0f;
    float pressure_dy = 0.0f;


    for(size_t y=0;y<height;++y)
    {
        for(size_t x=0;x<width;++x)
        {
            scale = _timeStep/(m_grid.density(x,y)*dx);
            //UpdateU
            if((x>=1 && m_grid.cell(x-1,y).label() == Label::FLUID) ||  m_grid.cell(x,y).label() == Label::FLUID )
            {
                if((x>=1 && m_grid.cell(x-1,y).label() == Label::SOLID) ||  m_grid.cell(x,y).label() == Label::SOLID )
                {
                    m_grid.cell(x,y).setVelocityU(usolid(x,y));
                }
                else
                {
                    pressure_dx = (m_grid.cell(x,y).pressure()-m_grid.cell(x-1,y).pressure());
                    if(m_grid.cell(x,y).label()==Label::EMPTY)
                        m_grid.cell(x,y).setVelocityU(m_grid.cell(x,y).velocityU());
                    else
                    m_grid.cell(x,y).setVelocityU(m_grid.cell(x,y).velocityU()-scale*pressure_dx);
                }
            }
            else
            {
                m_grid.cell(x,y).setVelocityU(0.0f);
            }
            //Update V
            if((y>=1 && m_grid.cell(x,y-1).label() == Label::FLUID) ||  m_grid.cell(x,y).label() == Label::FLUID )
            {
                if((y>=1 && m_grid.cell(x,y-1).label() == Label::SOLID) ||  m_grid.cell(x,y).label() == Label::SOLID )
                {
                      m_grid.cell(x,y).setVelocityV(vsolid(x,y));
                }
                else
                {
                    pressure_dy = (m_grid.cell(x,y).pressure()-m_grid.cell(x,y-1).pressure());
                    if(m_grid.cell(x,y).label()==Label::EMPTY)
                        m_grid.cell(x,y).setVelocityV(m_grid.cell(x,y).velocityV());
                    else
                        m_grid.cell(x,y).setVelocityV(m_grid.cell(x,y).velocityV()-scale*pressure_dy);
                }
            }
            else
            {
                m_grid.cell(x,y).setVelocityV(0.0f);
            }
        }
    }

}

//----------------------------------------------------------------------------------------------------------------------
//U direction solid velocity
float FluidSimulator::usolid(size_t _x, size_t _y)
{
    if(m_grid.cell(_x,_y).label() == Label::FLUID)
    {
        //Solid||Fluid
        return  m_grid.cell(_x,_y).velocityU();
    }

    //Fluid||Solid
    return -m_grid.cell(_x,_y).velocityU();


}

//----------------------------------------------------------------------------------------------------------------------
//V direction solid velocity
float FluidSimulator::vsolid(size_t _x, size_t _y)
{

    if(m_grid.cell(_x,_y).label() == Label::FLUID)
    {
        //Solid||Fluid
        return  m_grid.cell(_x,_y).velocityV();
    }

    //Fluid||Solid
    return -m_grid.cell(_x,_y).velocityV();
}

//----------------------------------------------------------------------------------------------------------------------
//main pressure solve routine
void FluidSimulator::pressureSolve(float _timeStep)
{
    size_t dim = m_grid.size();

    //Step1. Negative Divergence
    VectorXd b = negativeDivergence(_timeStep);

    //Step2. Set up the entries of A
    SparseMatrix<double,RowMajor> A = setUpMatrixA(_timeStep);

    //Step3. Conjugate gradient
    VectorXd p(dim);
    ConjugateGradient<SparseMatrix<double,RowMajor>> cg;
    cg.compute(A);

    //Step4. solve Ap=b
    p = cg.solve(b);
    if(cg.info() == Success)
        std::cout<< "SUCCESS: Convergence" << std::endl;
    else
    {
        std::cout << "FAILED: No Convergence" << std::endl;
    }

    //Step 5. Update Pressure Field
    updatePressureField(p);

    //Step6. Pressure Gradient Update
    pressureGradientUpdate(_timeStep);

}



/********************************VISUALIZATION DATA******************************************************/
//Prepares the data for the view. This data represents the status of the simulator

std::vector<FluidSimulator::vec3> FluidSimulator::velocityField(float _time)//doesn't take parameters
{
    std::vector<vec3> data;
    vec3 vertex;
    vertex.m_z = 0.0f;

    Grid::iterator cell_it = m_grid.begin();
    while(cell_it != m_grid.end())
    {

        vertex.m_x = cell_it->centre().m_x+ cosf(cell_it->velocityV()*3.14);
        vertex.m_y = cell_it->centre().m_y+ sinf(cell_it->velocityU()*3.14);
        //vertex.m_x = cell_it->centre().m_x + cell_it->velocity(cell_it->centre()).m_x;
        //vertex.m_y = cell_it->centre().m_y +cell_it->velocity(cell_it->centre()).m_y;
        data.push_back(vertex);
        cell_it++;
    }

    return data;
}

//----------------------------------------------------------------------------------------------------------------------
std::vector<FluidSimulator::vec3> FluidSimulator::activeCells(float _time)
{
    std::vector<vec3> data;

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

//----------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------
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
