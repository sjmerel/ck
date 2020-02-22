#include "ck/core/runningaverage.h"

namespace Cki
{


RunningAverage::RunningAverage(float* buf, int n) :
    m_buf(buf),
    m_size(n),
    m_index(0),
    m_count(0),
    m_sum(0.0f)
{}

void RunningAverage::sample(float value)
{
    // add new value
    m_sum += value;

    if (m_count == m_size)
    {
        // subtract old value
        m_sum -= m_buf[m_index];
    }

    m_buf[m_index] = value;

    ++m_index;
    if (m_index >= m_size)
    {
        m_index = 0;
    }

    if (m_count < m_size)
    {
        ++m_count;
    }
}

int RunningAverage::getCount() const
{
    return m_count;
}

float RunningAverage::getAverage() const
{
    if (m_count == 0)
    {
        return 0.0f; // not really right, but kinder than asserting
    }
    else
    {
        return m_sum / m_count;
    }
}

void RunningAverage::reset()
{
    m_index = 0;
    m_count = 0;
    m_sum = 0.0f;
}


}
