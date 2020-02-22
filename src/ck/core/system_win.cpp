#include "ck/core/system_win.h"
#include "ck/core/debug.h"
#include "ck/core/sse.h"
#include <new>
#include <stdlib.h>
#include <ShlObj.h>

namespace Cki
{



void SystemWin::init(const CkConfig& config)
{
    if (!s_instance)
    {
        s_instance = new (s_mem) SystemWin(config);
    }
}

void SystemWin::shutdown()
{
    if (s_instance)
    {
        s_instance->~SystemWin();
        s_instance = NULL;
    }
}


////////////////////////////////////////

SystemWin::SystemWin(const CkConfig& config) :
    System(config)
{
    // can't use __argv or _pgmptr, since those are NULL if wmain is used instead of main
    wchar_t wpath[Path::k_maxLen];
    GetModuleFileNameW(NULL, wpath, Path::k_maxLen);
	char path[Path::k_maxLen];
	WideCharToMultiByte(CP_UTF8, 0, wpath, -1, path, Path::k_maxLen, NULL, NULL);

    Path p(path);
    p.setParent();
    m_exeDir = p.getBuffer();

    GetTempPath(Path::k_maxLen, path);
    m_tempDir = path;

    wchar_t* wstr = NULL;
    SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &wstr);
    size_t nConverted;
    CK_VERIFY(0 == wcstombs_s(&nConverted, path, Path::k_maxLen, wstr, wcslen(wstr)));
    m_docDir = path;
    CoTaskMemFree(wstr);

    m_simd = Sse::isSupported();
}

SystemWin::~SystemWin()
{
}

const char* SystemWin::getExeDir() const
{
    return m_exeDir.getBuffer();
}

const char* SystemWin::getTempDir() const
{
    return m_tempDir.getBuffer();
}

const char* SystemWin::getDocumentsDir() const
{
    return m_docDir.getBuffer();
}

SystemWin* SystemWin::s_instance = NULL;
char SystemWin::s_mem[sizeof(SystemWin)];

}

