#pragma once

#include "ck/core/platform.h"
#include "ck/core/types.h"
#include "ck/audio/soundname.h"
#include "ck/audio/sampleinfo.h"

namespace Cki
{

class BinaryStream;
class MemoryFixup;

// TODO make class?
struct Sample
{
    Sample();

    SoundName name;  // 32 bytes
    SampleInfo info; // 28 bytes
    uint32 dataSize; // 4 bytes
    byte* data; // 8 bytes
#if CK_32_BIT
    uint32 pad;
#endif

    void write(BinaryStream&) const;
    void fixup(MemoryFixup&);
};


}
