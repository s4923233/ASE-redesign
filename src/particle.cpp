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

void Particle::addObserver(observer_ptr _observer)
{
    m_observer.push_back(_observer);
}

void Particle::removeObserver(observer_ptr _observer)
{

    bool found = false;
    std::vector<observer_ptr>::iterator obs_it = m_observer.begin();
    while(obs_it != m_observer.end())
    {
        if(*obs_it == _observer)
        {
            m_observer.erase(obs_it);
            found = true;
        }
        obs_it++;
    }
//    if(!found)
//        throw(std::bad_exception("Particle::removeObserver:\nObserver not part of the list\n"));

}

bool Particle::isObserver(observer_ptr _observer)
{
    bool found = false;
    for(auto observer: m_observer)
    {
        if(observer == _observer)
        {
            found = true;
        }
    }
    return found;
}


void Particle::notify(const Event _event)
{

    for(auto observer: m_observer)
    {
        if(observer == nullptr)
            continue;
        observer->onNotify(this,_event);
    }
}
