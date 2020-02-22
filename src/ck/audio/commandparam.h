#pragma once

#include "ck/core/platform.h"
#include "ck/core/debug.h"

namespace Cki
{


union CommandParam
{
    int32 intValue;
    float32 floatValue;
    void* addrValue;

    bool boolValue() const  { return (intValue != 0); }

    CommandParam() {}
    CommandParam(float32 value) { floatValue = value; }
    CommandParam(int32 value)   { intValue = value; }
    CommandParam(void* value)   { addrValue = (void*) value; }
};


}
