#pragma once
#include "ck/platform.h"

// defined in ck/platform.h:
//  CK_PLATFORM_IOS
//  CK_PLATFORM_TVOS
//  CK_PLATFORM_ANDROID
//  CK_PLATFORM_OSX
//  CK_PLATFORM_WIN
//  CK_PLATFORM_WP8
//  CK_PLATFORM_LINUX

#undef CK_BIG_ENDIAN
#undef CK_LITTLE_ENDIAN

#undef CK_ARCH_ARM
#undef CK_ARCH_X86
#undef CK_ARCH_MIPS

#undef CK_ARM_NEON
#undef CK_X86_SSE

#undef CK_32_BIT
#undef CK_64_BIT


#if CK_PLATFORM_IOS || CK_PLATFORM_OSX || CK_PLATFORM_TVOS
#  include <TargetConditionals.h>
#  if TARGET_RT_LITTLE_ENDIAN
#    define CK_LITTLE_ENDIAN 1
#  else
#    define CK_BIG_ENDIAN 1
#  endif
// CK_DEBUG=1 or CK_NDEBUG=1 set explicitly in Xcode project
#elif CK_PLATFORM_ANDROID
#  define CK_LITTLE_ENDIAN 1
#  if NDK_DEBUG // TODO use a different flag?
#    define CK_DEBUG 1
#  endif
#elif CK_PLATFORM_LINUX
#  define CK_LITTLE_ENDIAN 1
#elif CK_PLATFORM_WIN || CK_PLATFORM_WP8
#  define CK_LITTLE_ENDIAN 1
#endif

#if __arm__ || __arm64__ || __aarch64__ || defined(_M_ARM)
#  define CK_ARCH_ARM 1
#elif MIPSEL
#  define CK_ARCH_MIPS 1
#else
#  define CK_ARCH_X86 1
#endif

#if CK_PLATFORM_WIN || CK_PLATFORM_WP8
#  if _WIN64
#    define CK_64_BIT 1
#  else
#    define CK_32_BIT 1
#  endif
#else
#  if __LP64__
#    define CK_64_BIT 1
#  else
#    define CK_32_BIT 1
#  endif
#endif

#if __ARM_NEON__ || __aarch64__ || (CK_PLATFORM_WP8 && defined(_M_ARM))
// Note that on Android armeabi-v7a, this will only be defined in compilation units with .neon in 
// their name; on iOS it will be defined for all armv7* and arm64 builds.
#  define CK_ARM_NEON 1
#endif

#if __SSE__ || _M_IX86_FP || defined(_M_AMD64) || defined(_M_X64)
#  define CK_X86_SSE 1
#endif

// consistency checks
#if !CK_DEBUG && !CK_NDEBUG
#  error "Either CK_DEBUG or CK_NDEBUG must be set!"
#endif
#if !CK_BIG_ENDIAN && !CK_LITTLE_ENDIAN
#  error "Unknown byte order!"
#endif
#if !CK_32_BIT && !CK_64_BIT
#  error "Unknown pointer size!"
#endif
#if !CK_ARCH_ARM && !CK_ARCH_X86 && !CK_ARCH_MIPS
#  error "Unknown architecture!"
#endif


////////////////////////////////////////
// handy macros for short bits of platform-specific code

#if CK_DEBUG
#  define CK_DEBUG_ONLY(x) x
#else
#  define CK_DEBUG_ONLY(x) 
#endif

#if CK_PLATFORM_IOS
#  define CK_IOS_ONLY(x) x
#else
#  define CK_IOS_ONLY(x)
#endif

#if CK_PLATFORM_OSX
#  define CK_OSX_ONLY(x) x
#else
#  define CK_OSX_ONLY(x)
#endif

#if CK_PLATFORM_WIN
#  define CK_WIN_ONLY(x) x
#else
#  define CK_WIN_ONLY(x)
#endif

#if CK_PLATFORM_WP8
#  define CK_WP8_ONLY(x) x
#else
#  define CK_WP8_ONLY(x)
#endif

#if CK_PLATFORM_ANDROID
#  define CK_ANDROID_ONLY(x) x
#else
#  define CK_ANDROID_ONLY(x)
#endif

// for forward declaration in Objective-C++ files
#ifdef __OBJC__
#  define CK_DECLARE_CLASS @class
#else
#  define CK_DECLARE_CLASS class
#endif


