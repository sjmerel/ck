#pragma once

#include "ck/core/platform.h"
#include "ck/core/debug.h"

namespace Cki
{


class Stats
{
public:
    Stats();

    void sample(float);

    void reset();

    int getCount() const;

    float getMax() const;
    float getMin() const;
    float getSum() const;
    float getMean() const;

private:
    float m_sum;
    float m_max;
    float m_min;
    int m_count;
};


////////////////////////////////////////

inline
int Stats::getCount() const 
{ 
    return m_count; 
}

inline
float Stats::getMax() const 
{
    CK_ASSERT(m_count > 0); 
    return m_max; 
}

inline
float Stats::getMin() const 
{
    CK_ASSERT(m_count > 0); 
    return m_min; 
}

inline
float Stats::getSum() const 
{
    return m_sum; 
}

inline
float Stats::getMean() const 
{
    CK_ASSERT(m_count > 0); 
    return m_sum/m_count; 
}

}

