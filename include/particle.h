#ifndef PARTICLE_H
#define PARTICLE_H

#include <ngl/Vec2.h>
#include <vector>


//----------------------------------------------------------------------------------------------------------------------
/// @class Particle
/// @brief simple particle class to describe the particles in the fluid simulator
/// This class implements the particle to be used within a FLIP routine.
/// Each particle knows its position and velocity, it is not only a point in space.
///  @author Federico Leone
///  @version 2.0
///  @date
//----------------------------------------------------------------------------------------------------------------------

class Particle
{
    typedef ngl::Vec2 vec2;
public:
    size_t m_cellIndex;
    vec2 m_position;
    vec2 m_velocity;

    Particle();
    Particle(vec2 _pos);
    Particle(vec2 _pos, vec2 _velocity);
};

#endif // PARTICLE_H
