#pragma once

#include "ck/core/platform.h"
#include "ck/core/types.h"

namespace Cki
{

class BinaryStream;

struct Guid
{
    Guid() {}
    Guid(uint32, uint16, uint16, uint64);

    uint32 data1;
    uint16 data2;
    uint16 data3;
    byte data4[8];

    bool operator==(const Guid&) const;

    void write(BinaryStream&) const;
    void read(BinaryStream&);
};


}
