#pragma once

#include "ck/core/platform.h"
#include "ck/core/target.h"

namespace Cki
{


namespace ByteOrder
{
    enum Value
    {
        k_bigEndian,
        k_littleEndian,

#if CK_BIG_ENDIAN
        k_native = k_bigEndian
#else
        k_native = k_littleEndian
#endif
    };

    Value get(Target::Value);
}


}
