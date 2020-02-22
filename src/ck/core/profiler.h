#pragma once

#include "ck/core/platform.h"
#include "ck/core/timer.h"
#include "ck/core/stats.h"

namespace Cki
{


class Profiler
{
public:
    Profiler(const char* name, int maxSampleCount);
    ~Profiler();

    void start() 
    { 
        m_timer.start();
    }

    void stop()
    {
        m_timer.stop();
        record();
    }

private:
    const char* m_name;
    int m_count;
    int m_maxCount;
    Timer m_timer;
    Stats m_stats;

    void record();
    void print();
};


}
