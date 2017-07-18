#include "include/particle.h"

Particle::Particle()
{

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
        observer->onNotify(this,_event);
    }
}
