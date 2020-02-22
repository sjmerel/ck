#pragma once

#include "ck/core/platform.h"
#include "ck/core/types.h"

#include <stdarg.h>

namespace Cki
{


class String;

class TextWriter
{
public:
    static const char* k_endline;

    virtual ~TextWriter() {}

    // print string 
    virtual void write(const char*) = 0;

    // print string with argument list;
    void writef(const char* format, ...);

    // print string with va_list; 
    void writefv(const char* format, va_list);

    TextWriter& operator<<(char);
    TextWriter& operator<<(int32);
    TextWriter& operator<<(uint32);
    TextWriter& operator<<(int64);
    TextWriter& operator<<(uint64);
    TextWriter& operator<<(float);
    TextWriter& operator<<(const char*);
    TextWriter& operator<<(const String&);

private:
    enum { k_bufSize = 256 }; // strings longer than this will be truncated
    char m_buf[k_bufSize];
};


}

