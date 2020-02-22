#include "ck/core/stats.h"
#include "ck/core/debug.h"

namespace Cki
{


Stats::Stats() :
    m_sum(0.0f),
    m_max(0.0f),
    m_min(0.0f),
    m_count(0)
{}

void Stats::sample(float value)
{
    m_sum += value;
    if (m_count == 0 || value > m_max)
        m_max = value;
    if (m_count == 0 || value < m_min)
        m_min = value;
    ++m_count;
}

void Stats::reset()
{
    m_sum = 0.0f;
    m_count = 0;
}


}

