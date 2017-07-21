#ifndef CELL_H
#define CELL_H

#include <ngl/Vec2.h>
#include <vector>
#include <map>
#include <cassert>

#include "particle.h"

//----------------------------------------------------------------------------------------------------------------------
/// @enum used to describe what a cell contains
enum class Label{ EMPTY, SOLID, FLUID };
//----------------------------------------------------------------------------------------------------------------------
/// @enum used to describe the status of a cell
enum class Status{ ACTIVE, INACTIVE };

//----------------------------------------------------------------------------------------------------------------------
/// @class Cell
/// @brief Cell class. The cell is the basic unit of a grid. The cell has the main responsibilities of setting and
/// retreiving information from the simulation space. It implements the weighted transfer of quantities between the grid
/// and the particles. It is used to implement a MAC-Grid, for this reason the velocities are stored at the edges and the
/// pressure in the centre of the cell.
///
///  @author Federico Leone
///  @version 2.0
///  @date
//----------------------------------------------------------------------------------------------------------------------

class Cell
{
    typedef float* magnitude_ptr;
    typedef Cell* cell_ptr;
    typedef Particle* particle_ptr;
    typedef ngl::Vec2 vec2;

public:
    Cell();
    Cell(const size_t _index, const vec2 _minUV, const vec2 _maxUV, const std::array<magnitude_ptr,7>& _magnitude,const std::array<cell_ptr,4>& _neighbours);
    Cell(const Cell& _other);
    Cell& operator=(const Cell& _other);
    virtual ~Cell();

    //get methods
    Label label() const             {return m_label;}
    Status status() const           {return m_status;}
    size_t index() const             {return m_index;}

    float deltaU() const            {return m_deltaU;}
    float deltaV() const            {return m_deltaV;}

    float minU() const            {return m_minU;}
    float minV() const            {return m_minV;}
    float maxU() const            {return m_maxU;}
    float maxV() const            {return m_maxV;}

    vec2 centre() const           {return m_centre;}

    vec2 halfEdge(char _key)    {return m_halfEdge[_key];}

    size_t particleCount() const {return m_particleCount;}
    std::vector<int> neighbours() {return m_neighbourIndexList;}

    float density();

    float velocityU();
    float velocityV();
    float deltaVelocityU();
    float deltaVelocityV();
    float initialVelocityU();
    float initialVelocityV();

    vec2 velocity(const vec2 _point);
    vec2 deltaVelocity(const vec2 _point);

    float divergence();

    float pressure();


    //set methods
    void setLabel(const Label _label);
    void setStatus(const Status _status);

    void setInitialVelocityU(const float _magnitude);
    void setVelocityU(const float _magnitude);
    void setInitialVelocityV(const float _magnitude);
    void setVelocityV(const float _magnitude);

    void setPressure(const float _magnitude);

    void resetParticleCount() {m_particleCount = 0;}
    void incrementParticleCount() {++m_particleCount;}
    void setParticlePoolSize(size_t _size) {m_particlePoolSize = _size;}

    void initNeighbourIndexList();

private:
    Label m_label;
    Status m_status;
    size_t m_index;

    float m_deltaU;
    float m_deltaV;

    float m_minU;
    float m_minV;
    float m_maxU;
    float m_maxV;

    vec2 m_centre;

    std::map<char,vec2> m_halfEdge;

    size_t m_particlePoolSize; //used in density
    size_t m_particleCount;

    magnitude_ptr m_initialVelocityU;
    magnitude_ptr m_velocityU;
    magnitude_ptr m_deltaVelocityU;

    magnitude_ptr m_initialVelocityV;
    magnitude_ptr m_velocityV;
    magnitude_ptr m_deltaVelocityV;

    magnitude_ptr m_pressure;

    std::map<char,cell_ptr> m_neighbour;
    std::vector<int> m_neighbourIndexList;
    std::vector<particle_ptr> m_particles;
};

#endif // CELL_H
