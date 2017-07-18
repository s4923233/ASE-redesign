#ifndef OBSERVER_H
#define OBSERVER_H

#include "particle.h"

class Particle;
enum class EventType{JOIN_CELL,LEAVE_CELL};
struct Event
{
    EventType m_type;
    size_t m_cellIndex;
};

class Observer
{
    typedef Particle* particle_ptr;
public:
    virtual ~Observer();
    virtual void onNotify(const particle_ptr _entity, Event _event) = 0;
};

#endif
