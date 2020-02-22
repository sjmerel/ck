#pragma once

#include "ck/core/platform.h"
#include "ck/core/debug.h"

struct AudioStreamBasicDescription;

namespace Cki
{

#if CK_DEBUG
namespace Audio
{
    void printASBD(const AudioStreamBasicDescription& format);
}
#endif

#if CK_DEBUG
#  define CK_OSSTATUS_VERIFY(expr) do { OSStatus result = expr; if (result != 0) { Cki::g_debug.fail(#expr, __FILE__, __LINE__, "error = %d (%.4s)", result, &result); CK_BREAK(); } } while (false)
#else
#  define CK_OSSTATUS_VERIFY(expr) do { expr; } while (false)
#endif


}

