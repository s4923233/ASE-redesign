#ifndef FLUIDSIMULATOR_H
#define FLUIDSIMULATOR_H

typedef void (*FrameReadyHandler)(bool _newFrame);

class FluidSimulator
{
public:
    FluidSimulator();



private:
    void setFrameReady(const bool _frameReady);
    void registerFrameReadyHandler(FrameReadyHandler _handler);
private:

    bool m_frameReady = false;
    FrameReadyHandler event = nullptr;
};

#endif // FLUIDSIMULATOR_H
