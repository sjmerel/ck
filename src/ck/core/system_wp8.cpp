#include "ck/core/system_wp8.h"
#include "ck/core/debug.h"
#include "ck/core/sse.h"
#include <new>
#include <stdlib.h>

namespace Cki
{

namespace
{
    void getPath(Platform::String^ path, char* buf, int bufSize)
    {
        wchar_t wbuf[Path::k_maxLen+1];
        wcsncpy_s(wbuf, Path::k_maxLen+1, 
                path->Data(),
                Path::k_maxLen);

        size_t nConverted;
        CK_VERIFY(0 == wcstombs_s(&nConverted, buf, bufSize, wbuf, Path::k_maxLen));
    }
}


////////////////////////////////////////


void SystemWp8::init(const CkConfig& config)
{
    if (!s_instance)
    {
        s_instance = new (s_mem) SystemWp8(config);
    }
}

void SystemWp8::shutdown()
{
    if (s_instance)
    {
        s_instance->~SystemWp8();
        s_instance = NULL;
    }
}


////////////////////////////////////////

SystemWp8::SystemWp8(const CkConfig& config) :
    System(config)
{
    char buf[Path::k_maxLen+1];

    getPath(Windows::Storage::ApplicationData::Current->LocalFolder->Path, buf, Path::k_maxLen+1);
    m_localDir = buf;

    getPath(Windows::ApplicationModel::Package::Current->InstalledLocation->Path, buf, Path::k_maxLen+1);
    m_installationDir = buf;

    // only on 8.1:
//    getPath(Windows::Storage::ApplicationData::Current->TemporaryFolder->Path, buf, Path::k_maxLen+1);
//    m_tempDir = buf;
#if CK_ARCH_ARM
    m_simd = true; // all WP8 devices support NEON
#else
    m_simd = Sse::isSupported();
#endif
}

SystemWp8::~SystemWp8()
{
}

const char* SystemWp8::getInstallationDir() const
{
    return m_installationDir.getBuffer();
}

const char* SystemWp8::getLocalDir() const
{
    return m_localDir.getBuffer();
}

SystemWp8* SystemWp8::s_instance = NULL;
char SystemWp8::s_mem[sizeof(SystemWp8)];

}

