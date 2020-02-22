#pragma once

#include "ck/core/platform.h"
#include "ck/core/timer.h"
#include "ck/core/fixedarray.h"

namespace Cki
{


class GlitchFinder
{
public:
    GlitchFinder(const char* name, float threshMs);
    ~GlitchFinder();

    void sample();

private:
    const char* m_name;
    Timer m_timer;
    float m_threshMs;
    FixedArray<float, 16> m_times;
};


}
