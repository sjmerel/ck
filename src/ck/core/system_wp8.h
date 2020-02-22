#pragma once

#include "ck/core/platform.h"
#include "ck/core/system.h"
#include "ck/core/fixedstring.h"
#include "ck/core/path.h"

namespace Cki
{


class SystemWp8 : public System
{
public:
    static void init(const CkConfig&);
    static void shutdown();

    static SystemWp8* get() { return s_instance; }

    const char* getInstallationDir() const;
    const char* getLocalDir() const;

private:
    SystemWp8(const CkConfig&);
    virtual ~SystemWp8();

    FixedString<Path::k_maxLen> m_installationDir;
    FixedString<Path::k_maxLen> m_localDir;

    static SystemWp8* s_instance;
    static char s_mem[];
};


}


