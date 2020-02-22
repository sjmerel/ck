#pragma once

#include "ck/core/platform.h"
#include "ck/core/debug.h"

#include <SLES/OpenSLES.h>

namespace Cki
{


#if CK_DEBUG
namespace Audio
{
    const char* getSlError(SLresult);
    const char* getInterfaceName(SLInterfaceID id);
    const char* getObjectName(SLuint32 id);
    void printInterfaces(SLuint32 objId);
}
#endif

#if CK_DEBUG
#  define CK_SL_VERIFY(expr) do { SLresult result = expr; if (result != SL_RESULT_SUCCESS) { Cki::g_debug.fail(#expr, __FILE__, __LINE__, "%s (%d)", Cki::Audio::getSlError(result), result); CK_BREAK(); } } while (false)
#else
#  define CK_SL_VERIFY(expr) do { expr; } while (false)
#endif


}
