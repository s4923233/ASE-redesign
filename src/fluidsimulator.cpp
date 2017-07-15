#include "fluidsimulator.h"

FluidSimulator::FluidSimulator()
{

}

void FluidSimulator::setFrameReady(const bool _frameReady)
{
    this->m_frameReady = _frameReady;
    if(this->event != nullptr)
    {
        this->event(_frameReady);
    }
}

void FluidSimulator::registerFrameReadyHandler(FrameReadyHandler _handler)
{
    this->event = _handler;
}
