#pragma once

#include "ck/core/platform.h"

namespace Cki
{


union VarSample
{
    int32 fixedValue;
    float floatValue;

    VarSample() {}
    VarSample(float v) { floatValue = v; }
    VarSample(int32 v) { fixedValue = v; }
};


}
