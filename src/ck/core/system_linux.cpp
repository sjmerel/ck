#include "ck/core/system_linux.h"
#include "ck/core/sse.h"
#include "ck/core/debug.h"
#include <new>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pwd.h>

namespace Cki
{



void SystemLinux::init(const CkConfig& config)
{
    if (!s_instance)
    {
        s_instance = new (s_mem) SystemLinux(config);
    }
}

void SystemLinux::shutdown()
{
    if (s_instance)
    {
        s_instance->~SystemLinux();
        s_instance = NULL;
    }
}


////////////////////////////////////////

SystemLinux::SystemLinux(const CkConfig& config) :
    System(config)
{
    char exePath[Path::k_maxLen] = {0};
    readlink("/proc/self/exe", exePath, Path::k_maxLen);

    Path p(exePath);
    p.setParent();
    m_exeDir = p.getBuffer();

    const char* dir = getenv("TMPDIR");
    m_tempDir = (dir ? dir : P_tmpdir);

    dir = getenv("HOME");
    m_docDir = (dir ? dir : getpwuid(getuid())->pw_dir);

    m_simd = Sse::isSupported();
}

SystemLinux::~SystemLinux()
{
}

const char* SystemLinux::getExeDir() const
{
    return m_exeDir.getBuffer();
}

const char* SystemLinux::getTempDir() const
{
    return m_tempDir.getBuffer();
}

const char* SystemLinux::getDocumentsDir() const
{
    return m_docDir.getBuffer();
}

SystemLinux* SystemLinux::s_instance = NULL;
char SystemLinux::s_mem[sizeof(SystemLinux)];

}

