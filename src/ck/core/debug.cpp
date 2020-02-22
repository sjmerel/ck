#include "ck/core/debug.h"
#include "ck/core/fixedstring.h"
#include <stdio.h>

#if CK_PLATFORM_ANDROID
#  include <android/log.h>
#endif

namespace Cki
{


void DebugWriter::write(const char* msg)
{
#if CK_PLATFORM_ANDROID
    // android always appends \n, so we do some work to get around that
    writeAndroid(msg);
#else
    printf("%s", msg);

    // print to debugger
#  if CK_PLATFORM_WIN 
    OutputDebugString(msg); 
#  elif CK_PLATFORM_WP8
    // using OutputDebugStringW because OutputDebugStringA not allowed on WP8

    const int k_bufSize = 256;
    wchar_t buf[k_bufSize];
    buf[k_bufSize-1] = '\0';

    const char* p = msg;
    size_t n;
    do
    {
        n = mbstowcs(buf, p, k_bufSize-1);
        OutputDebugStringW(buf); 
        p += n;
    }
    while (n == k_bufSize - 1);
#  endif
#endif
}

#if CK_DEBUG
void DebugWriter::fail(const char* expr, const char* file, int line)
{
    fail(expr, file, line, (char*) NULL, NULL);
}

void DebugWriter::fail(const char* expr, const char* file, int line, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    write("--------------------------------------------------\n");
    write("ASSERTION FAILED: ");
    if (expr)
    {
        writef("\"%s\", ", expr);
    }

    // just use file basename
    const char* p = file;
    while (*p)
    {
        if (*p == '/' || *p == '\\')
        {
            file = p + 1;
        }
        ++p;
    }

    writef("%s, line %d\n", file, line);
    if (format)
    {
        writefv(format, args);
        write("\n");
    }
    write("--------------------------------------------------\n");

    va_end(args);
}
#endif // CK_DEBUG

////////////////////////////////////////

#if CK_PLATFORM_ANDROID
namespace
{
    // sets endStr to the terminating null, and sets endLine to the
    // last '\n' or null if none
    void findEndlines(const char* msg, const char*& endLine, const char*& endStr)
    {
        endStr = msg;
        while (*endStr)
        {
            if (*endStr == '\n')
            {
                endLine = endStr;
            }
            ++endStr;
        }
    }

    FixedString256 g_bufStr;
}

void DebugWriter::writeAndroid(const char* msg)
{
    const char* endline = NULL;
    const char* end = NULL;
    findEndlines(msg, endline, end);

    if (g_bufStr.isEmpty() && endline && (end == endline + 1))
    {
        // nothing left over in the buffer from previous logs, and
        // msg ends with \n, so just log it
        __android_log_write(ANDROID_LOG_INFO, "CK", msg);
    }
    else
    {
        if (endline)
        {
            // append msg up to last \n to buffer, and print it
            g_bufStr.append(msg, endline - msg + 1);
            __android_log_write(ANDROID_LOG_INFO, "CK", g_bufStr.getBuffer());
            g_bufStr.clear();
            if (end != endline + 1)
            {
                g_bufStr.append(endline + 1);
            }
        }
        else
        {
            g_bufStr.append(msg);
        }
    }
}
#endif // CK_PLATFORM_ANDROID

DebugWriter g_debug;


}
