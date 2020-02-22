#pragma once

#include "ck/core/platform.h"
#include "ck/audio/audionode.h"

namespace Cki
{


class SineNode : public AudioNode
{
public:
    SineNode(float freqHz, float amplitude);

    virtual BufferHandle process(int frames, bool& post);

#if CK_DEBUG
    virtual void print(int level);
#endif

private:
    int m_period;
    float m_amplitude;
    int m_pos;
};


}
