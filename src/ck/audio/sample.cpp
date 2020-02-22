#include "ck/audio/sample.h"
#include "ck/core/binarystream.h"
#include "ck/core/memoryfixup.h"

namespace Cki
{


Sample::Sample() :
    dataSize(0),
    data(NULL)
#if CK_32_BIT
    ,pad(0)
#endif
{
}

void Sample::write(BinaryStream& stream) const
{
    stream << name;
    stream << info;
    stream << dataSize;
    stream << (uint64) 0; // data

    int pos = stream.getPos();
    stream.setPos(stream.getSize());
    stream.write(data, dataSize);
    stream.setPos(pos);
}

void Sample::fixup(MemoryFixup& fixup)
{
    data = (byte*) fixup.getPointer();
    fixup.advance(dataSize);
}


}
