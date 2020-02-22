#pragma once

#include "ck/core/platform.h"
#include "ck/core/types.h"
#include "ck/core/debug.h"

#if CK_PLATFORM_ANDROID || CK_PLATFORM_LINUX
#  include <time.h>
#elif CK_PLATFORM_IOS || CK_PLATFORM_OSX || CK_PLATFORM_TVOS
#  include <mach/mach.h>
#  include <mach/mach_time.h>
#elif CK_PLATFORM_WIN || CK_PLATFORM_WP8
#  include <windows.h>
#endif

namespace Cki
{


class Timer
{
public:
    static void init();

    Timer();
    
    void start()
    {
        if (!isRunning())
        {
            m_startTick = getTick();
        }
    }

    void stop()
    {
        if (isRunning())
        {
            m_elapsedTicks += getTick() - m_startTick;
            m_startTick = 0;
        }
    }

    void reset();

    float getElapsedMs() const;

    bool isRunning() const { return m_startTick != 0; }

    static uint64 getTick() 
    {
#if CK_PLATFORM_IOS || CK_PLATFORM_OSX || CK_PLATFORM_TVOS
        return mach_absolute_time();
#elif CK_PLATFORM_ANDROID || CK_PLATFORM_LINUX
        timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return (ts.tv_sec - s_startTime.tv_sec) * 1000000000 + (ts.tv_nsec - s_startTime.tv_nsec);
#elif CK_PLATFORM_WIN || CK_PLATFORM_WP8
        uint64 now;
        QueryPerformanceCounter((LARGE_INTEGER*) &now);
        return now;
#endif
    }

    static uint64 getTick(float msFromNow);

private:
    uint64 m_startTick;
    uint64 m_elapsedTicks;
    static float s_msPerTick;
#if CK_PLATFORM_ANDROID || CK_PLATFORM_LINUX
    static timespec s_startTime;
#endif
};


}

