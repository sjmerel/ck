#pragma once

#include "ck/core/platform.h"
#include "ck/core/debug.h"


namespace Cki
{


#if CK_DEBUG
#  define CK_HRESULT_VERIFY(expr) do { HRESULT result = expr; if (FAILED(result)) { Cki::g_debug.fail(#expr, __FILE__, __LINE__, " (%d)", result); CK_BREAK(); } } while (false)
#else
#  define CK_HRESULT_VERIFY(expr) do { expr; } while (false)
#endif


}

