#pragma once

#include "ck/core/platform.h"
#include "ck/audio/graphsound.h"

namespace Cki
{


class StreamSound : public GraphSound
{
public:
    StreamSound();

    virtual void setPlayPosition(int);
    virtual void setPlayPositionMs(float);
    virtual void setLoop(int startFrame, int endFrame);

protected:
    virtual void updateSeek();
    virtual bool isReadySub() const;

private:
    int m_nextFrame;
    float m_nextMs;
};



}
