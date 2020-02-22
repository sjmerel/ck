#pragma once

#include "ck/core/platform.h"
#include "ck/core/types.h"

namespace Cki
{

class BinaryStream;

struct SampleInfo
{
    uint8 format;
    uint8 channels;
    uint16 sampleRate;
    int32 blocks; // -1 means unknown
    uint16 blockBytes;
    uint16 blockFrames;

    uint16 volume; 
    int16 pan; 

    uint32 loopStart; // default 0
    uint32 loopEnd;   // default blocks * blockFrames
    int16 loopCount; // default 0

    void reset();

    void write(BinaryStream&) const;
    void read(BinaryStream&);
    byte* read(byte* data);
};



}
