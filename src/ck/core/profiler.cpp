#include "ck/core/profiler.h"
#include "ck/core/debug.h"

namespace Cki
{


Profiler::Profiler(const char* name, int maxSampleCount) :
    m_name(name),
    m_count(0),
    m_maxCount(maxSampleCount),
    m_timer(),
    m_stats()
{
}

Profiler::~Profiler()
{
    if (m_count)
    {
        print();
    }
}

void Profiler::record()
{
    float ms = m_timer.getElapsedMs();
    m_stats.sample(ms);
    m_timer.reset();
    ++m_count;
    if (m_count == m_maxCount)
    {
        print();
        m_stats.reset();
        m_count = 0;
    }
}

void Profiler::print()
{
    CK_PRINT("%s: %f (%f-%f) ms, %d samples\n", m_name, m_stats.getMean(), m_stats.getMin(), m_stats.getMax(), m_count);
}


}
