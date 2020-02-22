#pragma once

#include "ck/core/platform.h"
#include "ck/core/system.h"
#include "ck/core/fixedstring.h"
#include "ck/core/path.h"

namespace Cki
{


class SystemWin : public System
{
public:
    static void init(const CkConfig&);
    static void shutdown();

    static SystemWin* get() { return s_instance; }

    const char* getExeDir() const;
    const char* getTempDir() const;
    const char* getDocumentsDir() const;

private:
    SystemWin(const CkConfig&);
    virtual ~SystemWin();

    FixedString<Path::k_maxLen> m_exeDir;
    FixedString<Path::k_maxLen> m_tempDir;
    FixedString<Path::k_maxLen> m_docDir;

    static SystemWin* s_instance;
    static char s_mem[];
};


}

