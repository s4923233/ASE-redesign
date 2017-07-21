#include "include/particle.h"

Particle::Particle()
{

}

Particle::Particle(vec2 _pos)
{
    this->m_position = _pos;
}

Particle::Particle(vec2 _pos, vec2 _velocity)
{
    this->m_position = _pos;
    this->m_velocity = _velocity;
}
