#ifndef CK_PCH_INCLUDE
#define CK_PCH_INCLUDE

#include "ck/core/platform.h"

////////////////////////////////////////

#if CK_PLATFORM_IOS || CK_PLATFORM_OSX || CK_PLATFORM_TVOS

#ifdef __OBJC__
#  include <Foundation/Foundation.h>
#endif

#include <AudioToolbox/AudioToolbox.h>
#include <CoreAudio/CoreAudioTypes.h>
#include <stdio.h>
#include <ctype.h>
#include <dirent.h>
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <math.h>
#include <memory.h>
#include <new>
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#endif

////////////////////////////////////////

#if CK_PLATFORM_ANDROID
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <stdio.h>
//#include <android/asset_manager.h>
//#include <android/asset_manager_jni.h>
#include <android/log.h>
#include <ctype.h>
#include <dirent.h>
#include <jni.h> // XXX causes crash with clang 3.4
#include <math.h>
#include <memory.h>
#include <new>
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#endif

////////////////////////////////////////

#if CK_PLATFORM_WIN 

#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT 1
#define _CRT_SECURE_NO_DEPRECATE 1

#ifndef _WIN32_WINNT
//#  define _WIN32_WINNT _WIN32_WINNT_WINXP	// Windows XP or later
#  define _WIN32_WINNT _WIN32_WINNT_VISTA	// Windows Vista or later
#endif						

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
// not xaudio2, as we're including different versions in audiohelpers_win*.cpp

#endif

////////////////////////////////////////

#if CK_PLATFORM_WP8

#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT 1
#define _CRT_SECURE_NO_DEPRECATE 1

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <xaudio2.h>

#endif

////////////////////////////////////////

#endif // CK_PCH_INCLUDE
