#pragma once

#include "ck/core/platform.h"
#include "encoder.h"

namespace Cki
{


class PcmI16Encoder : public Encoder
{
public:
    PcmI16Encoder(int channels);

    virtual int encode(const int16* inBuf, byte* outBuf, int frames, bool final); 
    virtual int getFramesPerBlock() const;
    virtual int getBytesPerBlock() const;
};


}

