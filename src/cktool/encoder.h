#pragma once

#include "ck/core/platform.h"
#include "ck/core/types.h"
#include "ck/audio/audioformat.h"

namespace Cki
{


class Encoder
{
public:
    Encoder(int channels);
    virtual ~Encoder() {}

    // writes encoded data into outBuf (size is assumed <= inBuf)
    // returns number of bytes written
    virtual int encode(const int16* inBuf, byte* outBuf, int frames, bool final) = 0; 

    virtual int getFramesPerBlock() const = 0;
    virtual int getBytesPerBlock() const = 0;

    static Encoder* create(AudioFormat::Value format, int channels);

protected:
    int m_channels;
};


}

