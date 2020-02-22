#include "ck/core/byteorder.h"

namespace Cki
{


namespace ByteOrder
{
    Value get(Target::Value)
    {
        return ByteOrder::k_littleEndian; // for now!
    }
}


}
