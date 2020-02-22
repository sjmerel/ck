#include "ck/core/timer.h"
#include "ck/core/debug.h"
#include "ck/core/thread.h"

namespace Cki
{

timespec Timer::s_startTime;

void Timer::init()
{
    s_msPerTick = 0.000001f;
    clock_gettime(CLOCK_MONOTONIC, &s_startTime);
}


}




