#pragma once

#include "ck/core/platform.h"

namespace Cki
{


class RunningAverage
{
public:
    RunningAverage(float* buf, int n);

    void sample(float);

    int getCount() const;
    float getAverage() const;
    float getSum() const { return m_sum; }

    void reset();

private:
    float* m_buf;
    int m_size;
    int m_index;
    int m_count;
    float m_sum;
};


}
