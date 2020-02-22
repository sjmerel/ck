#include "pcmi8encoder.h"
#include "ck/core/debug.h"
#include "ck/core/math.h"
#include <stdlib.h>

namespace Cki
{


PcmI8Encoder::PcmI8Encoder(int channels) :
    Encoder(channels)
{
}

//#define DITHER
int PcmI8Encoder::encode(const int16* inBuf, byte* outBuf, int frames, bool final)
{
    int samples = frames * m_channels;

#ifdef DITHER
    for (int i = 0; i < samples; ++i)
    {
        int8 dither;
        for (;;)
        {
            dither = rand() & 0xff;
            int prob = Math::max(0, 127 - Math::abs((int) dither));
            if ((rand() & 0x7f) <= prob)
            {
                break;
            }
        }

        outBuf[i] = (int8) ((inBuf[i] + dither) >> 8);
    }
#else
    for (int i = 0; i < samples; ++i)
    {
        outBuf[i] = (int8) (inBuf[i] >> 8);
    }
#endif
    return samples * sizeof(int8);
}

int PcmI8Encoder::getFramesPerBlock() const
{
    return 1;
}

int PcmI8Encoder::getBytesPerBlock() const
{
    return sizeof(int8) * m_channels;
}

}
