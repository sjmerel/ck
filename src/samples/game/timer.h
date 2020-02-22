#pragma once
#include "ck/platform.h"
#include <stdint.h>

#if CK_PLATFORM_OSX || CK_PLATFORM_IOS
#  include <mach/mach_time.h>
#elif CK_PLATFORM_ANDROID
#  include <time.h>
#endif


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

private:
    uint64_t m_startTick;
    uint64_t m_elapsedTicks;
    static float s_msPerTick;
#if CK_PLATFORM_ANDROID
    static timespec s_startTime;
#endif

    uint64_t getTick() const
    {
#if CK_PLATFORM_ANDROID
        timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        time_t s = (now.tv_sec - s_startTime.tv_sec);
        time_t ns = (now.tv_nsec - s_startTime.tv_nsec);
        return s * 1000000000LL + ns;
#elif CK_PLATFORM_IOS || CK_PLATFORM_OSX
        return mach_absolute_time();

#endif
    }
};



