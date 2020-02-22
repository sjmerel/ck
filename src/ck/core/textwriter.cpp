#include "ck/core/textwriter.h"
#include "ck/core/debug.h"
#include "ck/core/string.h"
#include <stdarg.h>
#include <stdio.h>

namespace Cki
{

void TextWriter::writef(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    writefv(format, args);
    va_end(args);
}

void TextWriter::writefv(const char* format, va_list args)
{
    // XXX this will fail if an argument is a wide char string that includes a 
    // character not supported in the current locale.
//    CK_VERIFY( _vsnprintf_s(m_buf, k_bufSize, _TRUNCATE, format, args) >= 0 );
    CK_VERIFY( vsnprintf(m_buf, k_bufSize, format, args) >= 0 );

    write(m_buf);
}

TextWriter& TextWriter::operator<<(char value)
{
    writef("%c", value);
    return *this;
}

TextWriter& TextWriter::operator<<(int32 value)
{
    writef("%d", value);
    return *this;
}

TextWriter& TextWriter::operator<<(uint32 value)
{
    writef("%u", value);
    return *this;
}

TextWriter& TextWriter::operator<<(int64 value)
{
    writef("%lld", value);
    return *this;
}

TextWriter& TextWriter::operator<<(uint64 value)
{
    writef("%llu", value);
    return *this;
}

TextWriter& TextWriter::operator<<(float value)
{
    writef("%f", value);
    return *this;
}

TextWriter& TextWriter::operator<<(const char* value)
{
    writef("%s", value);
    return *this;
}

TextWriter& TextWriter::operator<<(const String& value)
{
    writef("%s", value.getBuffer());
    return *this;
}

#if CK_PLATFORM_WIN || CK_PLATFORM_WP8
const char* TextWriter::k_endline = "\r\n";
#else
const char* TextWriter::k_endline = "\n";
#endif

} 

