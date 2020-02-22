#include "ck/audio/sinenode.h"
#include "ck/core/math.h"
#include "ck/core/fixedpoint.h"

namespace Cki
{


SineNode::SineNode(float freqHz, float amplitude) :
    m_amplitude(amplitude),
    m_pos(0)
{
    // XXX make period a whole number of samples
    m_period = Math::round(getSampleRate() / freqHz);
}

BufferHandle SineNode::process(int frames, bool& post)
{
    BufferHandle buf = acquireBuffer();
    int32* p = (int32*) buf.get();
    for (int frame = 0; frame < frames; ++frame)
    {
        int32 value = Fixed8_24::fromFloat(m_amplitude * Math::sin(m_pos * Math::k_2pi / m_period));
        *p++ = value;
        *p++ = value;
        ++m_pos;
    }
    if (m_pos >= m_period)
    {
        m_pos = m_pos % m_period;
    }
    return buf;
}

#if CK_DEBUG
void SineNode::print(int level)
{
    printImpl(level, "sine");
}
#endif

}
