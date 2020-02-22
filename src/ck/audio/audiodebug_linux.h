#pragma once

#include "ck/core/platform.h"
#include "ck/core/debug.h"
#include <alsa/asoundlib.h>

namespace Cki
{


#if CK_DEBUG
#  define CK_ALSA_VERIFY(expr) do { int result = expr; if (result < 0) { Cki::g_debug.fail(#expr, __FILE__, __LINE__, "%s (%d)", snd_strerror(result), result); CK_BREAK(); } } while (false)
#else
#  define CK_ALSA_VERIFY(expr) do { expr; } while (false)
#endif


}
