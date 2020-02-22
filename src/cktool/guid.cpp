#include "guid.h"
#include "ck/core/binarystream.h"

namespace Cki
{


Guid::Guid(uint32 _data1, uint16 _data2, uint16 _data3, uint64 _data4)
{
    data1 = _data1;
    data2 = _data2;
    data3 = _data3;
    for (int i = 0; i < 8; ++i)
    {
        data4[i] = (_data4 & (0xffLL << (7-i)*8)) >> (7-i)*8;
    }
}

bool Guid::operator==(const Guid& other) const
{
    if (data1 != other.data1 ||
        data2 != other.data2 ||
        data3 != other.data3)
    {
        return false;
    }

    for (int i = 0; i < 8; ++i)
    {
        if (data4[i] != other.data4[i])
        {
            return false;
        }
    }
           
    return true;
}

void Guid::write(BinaryStream& stream) const
{
    stream << data1;
    stream << data2;
    stream << data3;
    stream.write(data4, sizeof(data4));
}

void Guid::read(BinaryStream& stream)
{
    stream >> data1;
    stream >> data2;
    stream >> data3;
    stream.read(data4, sizeof(data4));
}


}
