#pragma once

#include "ck/core/platform.h"
#include "ck/core/textwriter.h"

namespace Cki
{


////////////////////////////////////////
// assert macros

#if CK_PLATFORM_WIN || CK_PLATFORM_WP8
#  define CK_BREAK() __debugbreak()
#else
#  define CK_BREAK() __builtin_trap()
#endif

#if CK_DEBUG
#  define CK_ASSERT(expr, ...)                  do { if (!(expr)) { Cki::g_debug.fail(#expr, __FILE__, __LINE__, ##__VA_ARGS__); CK_BREAK(); } } while (false)
#  define CK_FAIL(...)                          do { Cki::g_debug.fail(NULL, __FILE__, __LINE__, ##__VA_ARGS__); CK_BREAK(); } while (false)
#  define CK_VERIFY(expr, ...)                  CK_ASSERT(expr, ##__VA_ARGS__)
#else
#  define CK_ASSERT(expr, ...)                  do {} while (false)
#  define CK_FAIL(...)                          do {} while (false)
#  define CK_VERIFY(expr, ...)                  do { (void) (expr); } while (false)
#endif



////////////////////////////////////////
// print macro

#define CK_PRINT(format, ...)    do { Cki::g_debug.writef(format, ##__VA_ARGS__); } while (false)



////////////////////////////////////////
// static assert

// based on boost::static_assert;
// template specialization for StaticAssertFailure<false> is undefined, so applying sizeof() causes compiler error.
template <bool cond> struct StaticAssertFailure;
template <> struct StaticAssertFailure<true> {};

#define CK_STATIC_ASSERT(cond) enum { CK_CONCAT_VALUE(StaticAssert_Line, __LINE__) = sizeof(StaticAssertFailure<(bool)(cond)>) };




class DebugWriter : public TextWriter
{
public:
    virtual void write(const char*);

#if CK_DEBUG
    void fail(const char* expr, const char* file, int line);
    void fail(const char* expr, const char* file, int line, const char* format, ...);
#endif

private:
#if CK_PLATFORM_ANDROID
    void writeAndroid(const char*);
#endif
};

extern DebugWriter g_debug;


}
