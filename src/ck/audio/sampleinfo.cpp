#include "ck/audio/sampleinfo.h"
#include "ck/core/binarystream.h"
#include "ck/audio/audioformat.h"

namespace Cki
{


void SampleInfo::reset()
{
    format = (uint8) AudioFormat::k_unknown;
    channels = 0;
    sampleRate = 0;
    blocks = 0;
    blockBytes = 0;
    blockFrames = 0;
    volume = CK_UINT16_MAX;
    pan = 0;
    loopStart = 0;
    loopEnd = (uint32) -1;
    loopCount = 0;
}

void SampleInfo::write(BinaryStream& stream) const
{
    stream << format;
    stream << channels;
    stream << sampleRate;
    stream << blocks;
    stream << blockBytes;
    stream << blockFrames;
    stream << volume;
    stream << pan;
    stream << loopStart;
    stream << loopEnd;
    stream << loopCount;
    stream << (uint16) 0; // pad
}

void SampleInfo::read(BinaryStream& stream)
{
    stream >> format;
    stream >> channels;
    stream >> sampleRate;
    stream >> blocks;
    stream >> blockBytes;
    stream >> blockFrames;
    stream >> volume;
    stream >> pan;
    stream >> loopStart;
    stream >> loopEnd;
    stream >> loopCount;
    stream.skip(2); // pad
}

byte* SampleInfo::read(byte* data)
{
    return data;
}

}
