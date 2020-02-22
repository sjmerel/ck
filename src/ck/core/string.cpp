#include "ck/core/string.h"
#include "ck/core/debug.h"
#include "ck/core/math.h"
#include "ck/core/mem.h"
#include "ck/core/logger.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#if CK_PLATFORM_WIN || CK_PLATFORM_WP8
#  define STRNCASECMP _strnicmp
#  define STRCASECMP _stricmp
#else
#  define STRNCASECMP strncasecmp
#  define STRCASECMP strcasecmp
#endif

namespace Cki
{

String::String() :
    m_buf(NULL),
    m_bufSize(0),
    m_length(0),
    m_external(false)
{
}

String::String(int n) :
    m_buf(NULL),
    m_bufSize(0),
    m_length(0),
    m_external(false)
{
    reserve(n);
}

String::String(const char* s) :
    m_buf(NULL),
    m_bufSize(0),
    m_length(0),
    m_external(false)
{
    assign(s);
}

String::String(const String& s) :
    m_buf(NULL),
    m_bufSize(0),
    m_length(0),
    m_external(false)
{
    assign(s);
}

String::String(char* buf, int bufSize, External) :
    m_buf(buf),
    m_bufSize(bufSize),
    m_length(0),
    m_external(true)
{}

String::String(char* buf, External) :
    m_buf(buf),
    m_bufSize((int)strlen(buf) + 1),
    m_length(m_bufSize-1),
    m_external(true)
{}

String& String::operator=(const String& s)
{
    if (this != &s)
    {
        assign(s);
    }
    return *this;
}

String& String::operator=(const char* s)
{
    assign(s);
    return *this;
}

String::~String()
{
    if (!m_external)
    {
        Mem::free(m_buf);
    }
}

void String::printf(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    // note platform-specific vsnprintf behavior!
#if CK_PLATFORM_WIN || CK_PLATFORM_WP8
    // calculate size needed, not including null
    int size = _vscprintf(format, args); 
    reserve(size);
    int newLength = _vsnprintf(m_buf, m_bufSize, format, args); // returns -1 if truncated
    if (newLength < 0)
    {
        // truncated
        m_length = m_bufSize - 1;
        m_buf[m_length] = '\0';
    }
    else
    {
        m_length = newLength;
    }
#else
    int size = vsnprintf(m_buf, m_bufSize, format, args);
    if (size >= m_bufSize)
    {
        // not enough space, try to reserve more:
        int oldBufSize = m_bufSize;
        reserve(size);
        if (m_bufSize > oldBufSize)
        {
            // got some more space, try again
            va_end(args);
            va_start(args, format);
            vsnprintf(m_buf, m_bufSize, format, args);
        }
    }
    m_length = Math::min(size, m_bufSize - 1);
#endif

    va_end(args);
}

void String::resize(int len, char fill)
{
    CK_ASSERT(len >= 0);
    if (len < m_length)
    {
        eraseFrom(len);
    }
    else if (len > m_length)
    {
        reserve(len);
        int newLen = Math::min(len, m_bufSize - 1);
        CK_ASSERT(newLen >= m_length);
        if (newLen > m_length)
        {
            for (int i = m_length; i < newLen; ++i)
            {
                m_buf[i] = fill;
            }
            m_length = newLen;
            m_buf[m_length] = '\0';
        }
    }
}

void String::clear()
{
    m_length = 0;
    if (m_buf)
    {
        *m_buf = '\0';
    }
}

void String::reserve(int capacity)
{
    if (m_external)
    {
        if (capacity + 1 > m_bufSize)
        {
            CK_LOG_ERROR("string buffer too small for requested capacity!");
        }
    }
    else
    {
        // TODO allocate more aggressively (e.g. size is power of 2)?
        int newBufSize = capacity + 1; // terminating null
        if (newBufSize > m_bufSize)
        {
            char* newBuf = (char*) Mem::alloc(newBufSize);
            if (m_buf)
            {
                CK_ASSERT(m_bufSize > m_length);
                Mem::copy(newBuf, m_buf, m_bufSize);
                Mem::free(m_buf);
            }
            else
            {
                *newBuf = '\0';
            }
            m_buf = newBuf;
            m_bufSize = newBufSize;
        }
    }
}

int String::getCapacity() const
{
    if (m_bufSize == 0)
    {
        return 0;
    }
    else
    {
        return m_bufSize - 1;
    }
}

void String::compact()
{
    if (!m_external)
    {
        if (m_bufSize > m_length + 1)
        {
            int newBufSize = m_length + 1;
            char* newBuf = (char*) Mem::alloc(newBufSize);
            Mem::copy(newBuf, m_buf, newBufSize);
            Mem::free(m_buf);
            m_buf = newBuf;
            m_bufSize = newBufSize;
        }
    }
}

void String::assign(const String& s)
{
    assignImpl(s.m_buf, s.m_length);
}

void String::assign(const char* s)
{
    if (!s)
    {
        clear();
    }
    else
    {
        assignImpl(s, (int) strlen(s));
    }
}

void String::append(const String& s)
{
    appendImpl(s.m_buf, s.m_length);
}

void String::append(const char* s)
{
    if (s)
    {
        appendImpl(s, (int) strlen(s));
    }
}

void String::append(char c)
{
    appendImpl(&c, 1);
}

void String::append(const char* s, int n)
{
    CK_ASSERT(n >= 0);
    if (s)
    {
        appendImpl(s, Math::min((int) strlen(s), n));
    }
}

void String::append(const String& s, int n)
{
    CK_ASSERT(n >= 0);
    appendImpl(s.m_buf, Math::min(s.m_length, n));
}

const char& String::operator[](int i) const
{
    CK_ASSERT(i >= 0 && i < m_length);
    return m_buf[i];
}

char& String::operator[](int i)
{
    CK_ASSERT(i >= 0 && i < m_length);
    return m_buf[i];
}

const char* String::getBuffer() const
{
    return (m_buf ? m_buf : &s_null);
}

char* String::getBuffer()
{
    return (m_buf ? m_buf : &s_null);
}

bool String::equals(const String& s, bool ignoreCase) const
{
    return !compare(s, ignoreCase);
}

bool String::equals(const char* s, bool ignoreCase) const
{
    return !compare(s, ignoreCase);
}

int String::compare(const String& s, bool ignoreCase) const
{
    return compareImpl(s.getBuffer(), ignoreCase);
}

int String::compare(const char* s, bool ignoreCase) const
{
    return compareImpl(s, ignoreCase);
}

bool String::startsWith(const String& s, bool ignoreCase) const
{
    return startsWithImpl(s.m_buf, s.m_length, ignoreCase);
}

bool String::startsWith(const char* s, bool ignoreCase) const
{
    if (s)
    {
        return startsWithImpl(s, (int) strlen(s), ignoreCase);
    }
    else
    {
        return false;
    }
}

bool String::endsWith(const String& s, bool ignoreCase) const
{
    return endsWithImpl(s.m_buf, s.m_length, ignoreCase);
}

bool String::endsWith(const char* s, bool ignoreCase) const
{
    if (s)
    {
        return endsWithImpl(s, (int) strlen(s), ignoreCase);
    }
    else
    {
        return false;
    }
}

void String::makeLowerCase()
{
    for (int i = 0; i < m_length; ++i)
    {
        m_buf[i] = (char) tolower(m_buf[i]);
    }
}

void String::makeUpperCase()
{
    for (int i = 0; i < m_length; ++i)
    {
        m_buf[i] = (char) toupper(m_buf[i]);
    }
}

void String::erase(int pos, int n)
{
    CK_ASSERT(pos >= 0 && pos < m_length);
    CK_ASSERT(pos + n <= m_length);
    CK_ASSERT(n >= 0);
    if (n > 0)
    {
        int newLength = m_length - n;
        for (int i = pos; i < newLength; ++i)
        {
            m_buf[i] = m_buf[i+n];
        }
        m_length = newLength;
        m_buf[m_length] = '\0';
        CK_ASSERT(m_length >= 0);
    }
}

void String::eraseFrom(int pos)
{
    CK_ASSERT(pos >= 0 && pos < m_length);
    erase(pos, m_length - pos);
}

void String::eraseTo(int pos)
{
    CK_ASSERT(pos >= 0 && pos <= m_length);
    erase(0, pos);
}

void String::insert(int pos, const String& s)
{
    insertImpl(pos, s.m_buf, s.m_length);
}

void String::insert(int pos, const char* s)
{
    if (s)
    {
        insertImpl(pos, s, (int) strlen(s));
    }
}

void String::insert(int pos, char s)
{
    insertImpl(pos, &s, 1);
}

int String::find(char c) const
{
    return find(c, 0);
}

int String::find(char c, int start) const
{
    if (m_length > 0)
    {
        CK_ASSERT(start >= 0 && start < m_length);
        for (int i = start; i < m_length; ++i)
        {
            if (m_buf[i] == c)
            {
                return i;
            }
        }
    }
    return -1;
}

int String::rfind(char c) const
{
    return rfind(c, m_length - 1);
}

int String::rfind(char c, int start) const
{
    if (m_length > 0)
    {
        CK_ASSERT(start >= 0 && start < m_length);
        for (int i = start; i >= 0; --i)
        {
            if (m_buf[i] == c)
            {
                return i;
            }
        }
    }
    return -1;
}

bool String::operator==(const String& s) const
{
    return equals(s);
}

bool String::operator==(const char* s) const
{
    return equals(s);
}

bool String::operator!=(const String& s) const
{
    return !equals(s);
}

bool String::operator!=(const char* s) const
{
    return !equals(s);
}

bool String::operator<(const String& s) const
{
    return compare(s) < 0;
}

bool String::operator<(const char* s) const
{
    return compare(s) < 0;
}

bool String::operator>(const String& s) const
{
    return compare(s) > 0;
}

bool String::operator>(const char* s) const
{
    return compare(s) > 0;
}

bool String::operator<=(const String& s) const
{
    return !(compare(s) > 0);
}

bool String::operator<=(const char* s) const
{
    return !(compare(s) > 0);
}

bool String::operator>=(const String& s) const
{
    return !(compare(s) < 0);
}

bool String::operator>=(const char* s) const
{
    return !(compare(s) < 0);
}

bool String::equals(const char* s1, const char* s2, int n, bool ignoreCase)
{
    if (n >= 0)
    {
        if (ignoreCase)
        {
            return STRNCASECMP(s1, s2, n) == 0;
        }
        else
        {
            return strncmp(s1, s2, n) == 0;
        }
    }
    else
    {
        if (ignoreCase)
        {
            return STRCASECMP(s1, s2) == 0;
        }
        else
        {
            return strcmp(s1, s2) == 0;
        }
    }
}

int String::getLength(const char* s)
{
    return (int) strlen(s);
}

////////////////////////////////////////

char String::s_null = '\0';

void String::assignImpl(const char* str, int strLength)
{
    clear();
    appendImpl(str, strLength);
}

void String::appendImpl(const char* str, int strLength)
{
    if (str)
    {
        reserve(m_length + strLength);
        int charsToCopy = Math::min(strLength, m_bufSize - m_length - 1);
        CK_ASSERT(charsToCopy >= 0);
        if (charsToCopy)
        {
            Mem::copy(m_buf + m_length, str, charsToCopy);
            m_length += charsToCopy;
            m_buf[m_length] = '\0';
        }
        CK_ASSERT(m_length < m_bufSize);
    }
}

void String::insertImpl(int pos, const char* str, int strLength)
{
    if (str)
    {
        CK_ASSERT(pos >= 0 && pos <= m_length);
        reserve(m_length + strLength);

        // move chars to make space for insertion
        int newLength = Math::min(m_length + strLength, m_bufSize - 1);
        for (int i = newLength; i >= pos + strLength; --i)
        {
            m_buf[i] = m_buf[i-strLength];
        }

        // write inserted chars
        int charsToInsert = Math::min(strLength, m_bufSize - pos - 1);
        for (int i = 0; i < charsToInsert; ++i)
        {
            m_buf[pos + i] = str[i];
        }

        m_length = newLength;
        m_buf[m_length] = '\0';
    }
}

bool String::startsWithImpl(const char* str, int strLength, bool ignoreCase) const
{
    if (ignoreCase)
    {
        return STRNCASECMP(getBuffer(), str, strLength) == 0;
    }
    else
    {
        return strncmp(getBuffer(), str, strLength) == 0;
    }
}

bool String::endsWithImpl(const char* str, int strLength, bool ignoreCase) const
{
    if (strLength > m_length)
    {
        return false;
    }
    else
    {
        if (ignoreCase)
        {
            return STRNCASECMP(m_buf + m_length - strLength, str, strLength) == 0;
        }
        else
        {
            return strncmp(m_buf + m_length - strLength, str, strLength) == 0;
        }
    }
}

int String::compareImpl(const char* s, bool ignoreCase) const
{
    if (ignoreCase)
    {
        return STRCASECMP(getBuffer(), s);
    }
    else
    {
        return strcmp(getBuffer(), s);
    }
}


}
