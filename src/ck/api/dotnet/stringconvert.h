#pragma once
#include "ck/core/string.h"
#include "ck/core/debug.h"
#include <windows.h>

namespace CricketTechnology
{
namespace Audio
{

template <int N = 256>
class StringConvert
{
public:
    StringConvert(Platform::String^ s)
    {
        int len = WideCharToMultiByte(CP_UTF8, 0, s->Data(), s->Length(), m_buf, N, NULL, NULL);
        CK_ASSERT(len);
        m_buf[len] = '\0';
    }

    const char* getCString() const { return m_buf; }

    static Platform::String^ newPlatformString(const char* s)
    {
        wchar_t buf[N];

        int len = MultiByteToWideChar(CP_UTF8, 0, s, Cki::String::getLength(s), buf, N);
        CK_ASSERT(len);

        return ref new Platform::String(buf, len);
    }

private:
    char m_buf[N];
};

}
}

