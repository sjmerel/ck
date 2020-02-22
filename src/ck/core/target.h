#pragma once

#include "ck/core/platform.h"

namespace Cki
{


//
// Target types
//
// This is mostly for specializing file formats, so the values may not
// corresponds exactly to the CK_PLATFORM_* defines.  For example, 
// the same CK_PLATFORM define may include both 32- and 64-bit versions of
// an operating system, or both big- and little-endian versions, but those
// would have different enums.

namespace Target
{
    enum Value
    {
        k_ios     = (1 << 0),
        k_android = (1 << 1),
        k_osx     = (1 << 2),
        k_win     = (1 << 3),
        k_wp8     = (1 << 4),
        k_linux   = (1 << 5),
        k_tvos    = (1 << 6),

#if CK_PLATFORM_IOS
        k_current = k_ios,
#elif CK_PLATFORM_ANDROID
        k_current = k_android,
#elif CK_PLATFORM_OSX
        k_current = k_osx,
#elif CK_PLATFORM_WIN
        k_current = k_win,
#elif CK_PLATFORM_WP8
        k_current = k_wp8,
#elif CK_PLATFORM_LINUX
        k_current = k_linux,
#elif CK_PLATFORM_TVOS
        k_current = k_tvos,
#endif

        k_all = (k_ios | k_android | k_osx | k_win | k_wp8 | k_linux | k_tvos)

    };

    const char* getName(Value);
}


}
