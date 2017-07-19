#include "cell.h"
#include <limits.h>

Cell::Cell()
{
    setLabel(Label::EMPTY);
    setStatus(Status::INACTIVE);
    m_index = UINT_MAX;

    m_minU = 0.0f;
    m_minV = 0.0f;
    m_maxU = 0.0f;
    m_maxV = 0.0f;

    m_deltaU = m_maxU - m_minU;
    m_deltaV = m_maxV - m_minV;

    m_centre.m_x = m_minU + m_deltaU/2;
    m_centre.m_y = m_minV + m_deltaV/2;

    m_halfEdge['N'] = vec2(m_centre.m_x,m_maxV);
    m_halfEdge['S'] = vec2(m_centre.m_x,m_minV);
    m_halfEdge['E'] = vec2(m_maxU,m_centre.m_y);
    m_halfEdge['W'] = vec2(m_minU,m_centre.m_y);

    m_particlePoolSize = 0;

    m_initialVelocityU = nullptr;
    m_velocityU = nullptr;
    m_deltaVelocityU = nullptr;
    m_initialVelocityV = nullptr;
    m_velocityV = nullptr;
    m_deltaVelocityV = nullptr;
    m_pressure = nullptr;

    m_neighbour['N'] = nullptr;
    m_neighbour['S'] = nullptr;
    m_neighbour['E'] = nullptr;
    m_neighbour['W'] = nullptr;

    m_particles.clear();
}


Cell::Cell(const size_t _index, const vec2 _minUV, const vec2 _maxUV, const std::array<magnitude_ptr,7>& _magnitude,const std::array<cell_ptr,4>& _neighbour)
{
    setLabel(Label::EMPTY);
    setStatus(Status::INACTIVE);
    m_index = _index;

    m_minU = _minUV.m_x;
    m_minV = _minUV.m_y;
    m_maxU = _maxUV.m_x;
    m_maxV = _maxUV.m_y;

    m_deltaU = m_maxU - m_minU;
    m_deltaV = m_maxV - m_minV;

    m_centre.m_x = m_minU + m_deltaU/2;
    m_centre.m_y = m_minV + m_deltaV/2;

    m_halfEdge['N'] = vec2(m_centre.m_x,m_maxV);
    m_halfEdge['S'] = vec2(m_centre.m_x,m_minV);
    m_halfEdge['E'] = vec2(m_maxU,m_centre.m_y);
    m_halfEdge['W'] = vec2(m_minU,m_centre.m_y);

    m_particlePoolSize = 0;

    m_initialVelocityU = _magnitude[0];
    m_velocityU = _magnitude[1];
    m_deltaVelocityU = _magnitude[2];
    m_initialVelocityV = _magnitude[3];
    m_velocityV = _magnitude[4];
    m_deltaVelocityV = _magnitude[5];
    m_pressure = _magnitude[6];

    m_neighbour['N'] = _neighbour[0];
    m_neighbour['S'] = _neighbour[1];
    m_neighbour['E'] = _neighbour[2];
    m_neighbour['W'] = _neighbour[3];

    m_particles.clear();
}

Cell::Cell(const Cell& _other)
{
    this->m_label = _other.m_label;
    this->m_status = _other.m_status;
    this->m_index = _other.m_index;

    this->m_minU = _other.m_minU;
    this->m_minV = _other.m_minV;
    this->m_maxU = _other.m_maxU;
    this->m_maxV = _other.m_maxV;

    this->m_deltaU = _other.m_deltaU;
    this->m_deltaV = _other.m_deltaU;

    this->m_centre = _other.m_centre;

    this->m_halfEdge = _other.m_halfEdge;

    this->m_particlePoolSize = _other.m_particlePoolSize;

    this->m_initialVelocityU = _other.m_initialVelocityU;
    this->m_velocityU =  _other.m_velocityU;
    this->m_deltaVelocityU = _other.m_deltaVelocityU;
    this->m_initialVelocityV = _other.m_initialVelocityV;
    this->m_velocityV = _other.m_velocityV;
    this->m_deltaVelocityV = _other.m_deltaVelocityV;
    this->m_pressure = _other.m_pressure;

    this->m_neighbour = _other.m_neighbour;
    this->m_particles = _other.m_particles;
}

Cell& Cell::operator=(const Cell& _other)
{
    assert(this != &_other);

    this->m_label = _other.m_label;
    this->m_status = _other.m_status;
    this->m_index = _other.m_index;

    this->m_minU = _other.m_minU;
    this->m_minV = _other.m_minV;
    this->m_maxU = _other.m_maxU;
    this->m_maxV = _other.m_maxV;

    this->m_deltaU = _other.m_deltaU;
    this->m_deltaV = _other.m_deltaU;

    this->m_centre = _other.m_centre;

    this->m_halfEdge = _other.m_halfEdge;

    this->m_particlePoolSize = _other.m_particlePoolSize;

    this->m_initialVelocityU = _other.m_initialVelocityU;
    this->m_velocityU =  _other.m_velocityU;
    this->m_deltaVelocityU = _other.m_deltaVelocityU;
    this->m_initialVelocityV = _other.m_initialVelocityV;
    this->m_velocityV = _other.m_velocityV;
    this->m_deltaVelocityV = _other.m_deltaVelocityV;
    this->m_pressure = _other.m_pressure;

    this->m_neighbour = _other.m_neighbour;
    this->m_particles = _other.m_particles;

    return *this;
}

Cell::~Cell(){}

float Cell::density()
{
    return static_cast<float>(m_particles.size())
            /static_cast<float>(m_particlePoolSize);
}

float Cell::velocityU()
{
    return *m_velocityU;
}

float Cell::velocityV()
{
    return *m_velocityV;
}

float Cell::deltaVelocityU()
{
    return *m_deltaVelocityU;
}

float Cell::deltaVelocityV()
{
    return *m_deltaVelocityV;
}

float Cell::initialVelocityU()
{
    return *m_initialVelocityU;
}

float Cell::initialVelocityV()
{
    return *m_initialVelocityV;
}

//to change. Use map for velocities, instead
Cell::vec2 Cell::velocity(const vec2 _point)
{
    if ((_point.m_x<minU())||(_point.m_x>m_maxU))
        throw(std::range_error("Error: Point outside cell boundaries"));
    if ((_point.m_y<minV())||(_point.m_y>m_maxV))
        throw(std::range_error("Error: Point outside cell boundaries"));

    float alphaU = (_point.m_x-m_minU)/m_deltaU;
    float alphaV = (_point.m_y-m_minV)/m_deltaV;


    float E_velocityU = m_neighbour['E'] == nullptr ? 0.0f: m_neighbour['E']->velocityU();
    float N_velocityV = m_neighbour['N'] == nullptr ? 0.0f: m_neighbour['N']->velocityV();
    vec2 result;

    result.m_x = (1-alphaU)*velocityU() + alphaU*E_velocityU;//velocityW() + alphaU*velocityE();
    result.m_y= (1-alphaV)*velocityV() + alphaU*N_velocityV;//velocityS() + alphaV*velocityN();

    return result;
}

Cell::vec2 Cell::deltaVelocity(const vec2 _point)
{
    if ((_point.m_x<minU())||(_point.m_x>m_maxU))
        throw(std::range_error("Error: Point outside cell boundaries"));
    if ((_point.m_y<minV())||(_point.m_y>m_maxV))
        throw(std::range_error("Error: Point outside cell boundaries"));

    float alphaU = (_point.m_x-m_minU)/m_deltaU;
    float alphaV = (_point.m_y-m_minV)/m_deltaV;

    float E_deltaVelocityU = m_neighbour['E'] == nullptr ? 0.0f: m_neighbour['E']->deltaVelocityU();
    float N_deltaVelocityV = m_neighbour['N'] == nullptr ? 0.0f: m_neighbour['N']->deltaVelocityV();
    vec2 result;

    result.m_x = (1-alphaU)*deltaVelocityU() + alphaU*E_deltaVelocityU;
    result.m_y= (1-alphaV)*deltaVelocityV() + alphaU*N_deltaVelocityV;

    return result;
}

float Cell::divergence()
{
    float E_velocityU = m_neighbour['E'] == nullptr ? 0.0f: m_neighbour['E']->velocityU();
    float N_velocityV = m_neighbour['N'] == nullptr ? 0.0f: m_neighbour['N']->velocityV();

    float uDivergence = E_velocityU-velocityU();
    float vDivergence = N_velocityV-velocityV();

    return uDivergence+vDivergence;
}

float Cell::pressure()
{
    return *m_pressure;
}

//set methods
void Cell::setLabel(const Label _label)
{
    m_label = _label;
}

void Cell::setStatus(const Status _status)
{
    m_status = _status;
}


//updates velocityU as well
void Cell::setInitialVelocityU(const float _magnitude)
{
    *m_initialVelocityU = _magnitude;
    setVelocityU(_magnitude);
}

void Cell::setVelocityU(const float _magnitude)
{
    *m_velocityU = _magnitude;
}

//updates velocityV as well
void Cell::setInitialVelocityV(const float _magnitude)
{
    *m_initialVelocityV = _magnitude;
    setVelocityV(_magnitude);
}

void Cell::setVelocityV(const float _magnitude)
{
    *m_velocityV = _magnitude;
}

//deltavelocity must be updated from Grid

void Cell::setPressure(const float _magnitude)
{
    *m_pressure = _magnitude;
}


void Cell::onNotify(const particle_ptr _entity, Event _event)
{

    EventType eventType = _event.m_type;
    switch(eventType)
    {
    case EventType::JOIN_CELL:
    {
        //if the cell joined is this cell and not a neighbour
       if(_event.m_cellIndex == m_index)
       {
           m_particles.push_back(_entity);
           setStatus(Status::ACTIVE);

           //let the neighbour observe the particle
           //so they can receive a notification from the particle
           for(auto &neighbour : m_neighbour)
           {
               cell_ptr c = neighbour.second;
               if(!_entity->isObserver(c))
               {
                   _entity->addObserver(c);
               }
           }
       }

       break;
    }
    case EventType::LEAVE_CELL:
    {
        //if the particle left this cell
        if(_event.m_cellIndex == m_index)
        {
            //remove the particle from the list of guests
            std::vector<particle_ptr>::iterator particle_it = m_particles.begin();
            while(particle_it != m_particles.end())
            {
                if(*particle_it == _entity)
                {
                    m_particles.erase(particle_it);
                }
                particle_it++;
            }
            //if there is no other particle left in this cell, then turn to inactive
            if(m_particles.size() == 0)
                setStatus(Status::INACTIVE);
        }
        else //the particle left a neighbour
        {
            //if no one of the neighbour is active
            bool removeObserver = true;
            for(auto &neighbour : m_neighbour)
            {
                if(neighbour.second == 0)
                    continue;
                cell_ptr c = neighbour.second;
                if(c->status() == Status::ACTIVE)
                {
                    removeObserver = false;
                }
            }

            //then remove the cell from the observers list
            if(removeObserver)
                _entity->removeObserver(this);
        }
        break;
    }
    }

}
