#include "pcmi16encoder.h"
#include "ck/core/mem.h"

namespace Cki
{


PcmI16Encoder::PcmI16Encoder(int channels) :
    Encoder(channels)
{
}

int PcmI16Encoder::encode(const int16* inBuf, byte* outBuf, int frames, bool final)
{
    int bytes = frames * m_channels * sizeof(int16);
    Mem::copy(outBuf, inBuf, bytes);
    return bytes;
}

int PcmI16Encoder::getFramesPerBlock() const
{
    return 1;
}

int PcmI16Encoder::getBytesPerBlock() const
{
    return sizeof(int16) * m_channels;
}


}
