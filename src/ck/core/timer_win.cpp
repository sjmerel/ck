#include "ck/core/timer.h"
#include "ck/core/debug.h"

namespace Cki
{


void Timer::init()
{
    uint64 ticksPerSec;
    QueryPerformanceFrequency((LARGE_INTEGER*) &ticksPerSec);
    s_msPerTick = 1000.0f / ticksPerSec;
}



}


