#pragma once

#include "ck/core/platform.h"
#include "encoder.h"

namespace Cki
{


class PcmI8Encoder : public Encoder
{
public:
    PcmI8Encoder(int channels);

    virtual int encode(const int16* inBuf, byte* outBuf, int frames, bool final); 
    virtual int getFramesPerBlock() const;
    virtual int getBytesPerBlock() const;
};


}

