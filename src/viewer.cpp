#include "viewer.h"

Viewer::Viewer(const FluidSimulator& _fluidSimulator)
{
    setModel(_fluidSimulator);
}

Viewer::~Viewer(){}

void Viewer::setModel(const FluidSimulator& _fluidSimulator)
{
    this->m_fluidSimulator = _fluidSimulator;
}

void Viewer::render()
{

}
