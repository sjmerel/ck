#pragma once

#include "ck/core/platform.h"
#include "ck/core/system.h"
#include "ck/core/fixedstring.h"
#include "ck/core/path.h"

namespace Cki
{


class SystemLinux : public System
{
public:
    static void init(const CkConfig&);
    static void shutdown();

    static SystemLinux* get() { return s_instance; }

    const char* getExeDir() const;
    const char* getTempDir() const;
    const char* getDocumentsDir() const;

private:
    SystemLinux(const CkConfig&);
    virtual ~SystemLinux();

    FixedString<Path::k_maxLen> m_exeDir;
    FixedString<Path::k_maxLen> m_tempDir;
    FixedString<Path::k_maxLen> m_docDir;

    static SystemLinux* s_instance;
    static char s_mem[];
};


}


