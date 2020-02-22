#include "ck/core/timer.h"
#include "ck/core/debug.h"

namespace Cki
{


void Timer::init()
{
    mach_timebase_info_data_t info;
    CK_VERIFY( mach_timebase_info(&info) == 0 );
    s_msPerTick = 1e-6f * (float) info.numer / (float) info.denom;
}


}

