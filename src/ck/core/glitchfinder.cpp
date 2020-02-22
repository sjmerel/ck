#include "ck/core/glitchfinder.h"
#include "ck/core/debug.h"

#include "ck/core/fixedarray.cpp" // template instantiation
#include "ck/core/array.cpp" // template instantiation

namespace Cki
{


GlitchFinder::GlitchFinder(const char* name, float threshMs) :
    m_name(name),
    m_threshMs(threshMs)
{
    m_timer.start();
}

GlitchFinder::~GlitchFinder()
{
    sample();
    float ms = m_times.last();
    if (ms >= m_threshMs)
    {
        CK_PRINT("**** glitch (%s): %.3f", m_name, ms);
        int count = m_times.getSize();
        if (count > 0)
        {
            CK_PRINT(": %.3f ", m_times[0]);
            for (int i = 0; i < count-1; ++i)
            {
                CK_PRINT("%.3f ", m_times[i+1] - m_times[i]);
            }
        }
        CK_PRINT("\n");
    }
}

void GlitchFinder::sample()
{
    m_times.append(m_timer.getElapsedMs());
}


}
