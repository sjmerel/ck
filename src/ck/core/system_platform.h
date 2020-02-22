#pragma once

#include "ck/core/platform.h"

#if CK_PLATFORM_IOS
#  include "ck/core/system_ios.h"
#elif CK_PLATFORM_TVOS
#  include "ck/core/system_tvos.h"
#elif CK_PLATFORM_ANDROID
#  include "ck/core/system_android.h"
#elif CK_PLATFORM_WIN
#  include "ck/core/system_win.h"
#elif CK_PLATFORM_OSX
#  include "ck/core/system_osx.h"
#elif CK_PLATFORM_WP8
#  include "ck/core/system_wp8.h"
#elif CK_PLATFORM_LINUX
#  include "ck/core/system_linux.h"
#endif

namespace Cki
{

#if CK_PLATFORM_IOS 
typedef SystemIos SystemPlatform;
#elif CK_PLATFORM_TVOS 
typedef SystemTvos SystemPlatform;
#elif CK_PLATFORM_OSX
typedef SystemOsx SystemPlatform;
#elif CK_PLATFORM_ANDROID
typedef SystemAndroid SystemPlatform;
#elif CK_PLATFORM_WIN
typedef SystemWin SystemPlatform;
#elif CK_PLATFORM_WP8
typedef SystemWp8 SystemPlatform;
#elif CK_PLATFORM_LINUX
typedef SystemLinux SystemPlatform;
#endif


}
