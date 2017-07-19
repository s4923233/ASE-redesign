#ifndef PARTICLE_H
#define PARTICLE_H

#include <ngl/Vec2.h>
#include <vector>
#include "observer.h"

class Observer;
struct Event;

class Particle
{
    typedef ngl::Vec2 vec2;
    typedef Observer* observer_ptr;
public:
    size_t m_cellIndex;
    vec2 m_position;
    vec2 m_velocity;

    Particle();
    Particle(vec2 _pos);
    Particle(vec2 _pos, vec2 _velocity);

    void addObserver(observer_ptr _observer);
    void removeObserver(observer_ptr _observer);
    bool isObserver(observer_ptr _observer);

    void notify(const Event _event);
private:
    std::vector<observer_ptr> m_observer;
};

#endif // PARTICLE_H
