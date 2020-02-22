#pragma once

#include "ck/core/platform.h"
#include "ck/core/system.h"
#include "ck/core/path.h"

namespace Cki
{


class SystemIos : public System
{
public:
    static void init(const CkConfig&);
    static void shutdown();

    static SystemIos* get() { return s_instance; }

    const char* getResourceDir() const;
    const char* getTempDir() const;
    const char* getDocumentsDir() const;

#if CK_PLATFORM_IOS
    // versionString should be something like "3.0", "4.3.1", etc
    bool isVersionGreaterThanOrEqualTo(const char* versionString) const;
#endif

private:
    SystemIos(const CkConfig&);
    virtual ~SystemIos();

    char m_resourceDir[Path::k_maxLen];
    char m_tempDir[Path::k_maxLen];
    char m_docDir[Path::k_maxLen];
    
    static SystemIos* s_instance;
    static char s_mem[];
};


}
