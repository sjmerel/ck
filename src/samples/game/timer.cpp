#include "timer.h"


Timer::Timer() :
    m_startTick(0),
    m_elapsedTicks(0)
{}

void Timer::reset()
{
    m_elapsedTicks = 0;
    if (isRunning())
    {
        m_startTick = getTick();
    }
}

float Timer::getElapsedMs() const
{
    uint64_t elapsed = m_elapsedTicks;
    if (isRunning())
    {
        elapsed += getTick() - m_startTick;
    }
    return s_msPerTick * elapsed;
}

////////////////////////////////////////

float Timer::s_msPerTick = 0.0f;


void Timer::init()
{
#if CK_PLATFORM_ANDROID
    clock_gettime(CLOCK_MONOTONIC, &s_startTime);

    // ticks are nanoseconds
    s_msPerTick = 0.000001f;
#elif CK_PLATFORM_IOS || CK_PLATFORM_OSX
    mach_timebase_info_data_t info;
    mach_timebase_info(&info);
    s_msPerTick = 1e-6f * (float) info.numer / (float) info.denom;
#endif
}

#if CK_PLATFORM_ANDROID
timespec Timer::s_startTime;
#endif


