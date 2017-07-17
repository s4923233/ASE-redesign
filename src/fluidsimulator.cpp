#include "fluidsimulator.h"
#include <math.h>
#include <random>


FluidSimulator::FluidSimulator()
{
    m_grid= Grid(5,5,15,15);
    initCellCentres();
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

    std::default_random_engine generator(_time);
    std::uniform_int_distribution<int> distribution(0,m_cellCentres.size()-1);

    int activeCells = distribution(generator);

    for (int i=0; i<activeCells; ++i)
    {
        data.push_back(m_cellCentres[distribution(generator)]);

    }

    return data;

}
/**************************************************************************************/

/********************************STUB METHOD!! SUBSTITUTE!!!******************************************************/
std::vector<FluidSimulator::vec3> FluidSimulator::particles()
{
    std::vector<vec3> data;
    data = m_cellCentres;
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
