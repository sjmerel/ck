#pragma once

#include "ck/core/platform.h"

namespace Cki
{


namespace AudioFormat
{
    enum Value
    {
        k_unknown = -1,

        k_pcmI16 = 0,
        k_pcmI8,
        k_adpcm,
        k_pcmF32
    };
}


}
