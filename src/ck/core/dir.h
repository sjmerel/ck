#pragma once

#include "ck/core/platform.h"
#include "ck/core/path.h"

#if CK_PLATFORM_WIN || CK_PLATFORM_WP8
#  include <windows.h>
#else
#  include <dirent.h>
#endif


namespace Cki
{


class Dir
{
public:
    Dir(const char* path);
    ~Dir();

    bool isValid();

    const char* getChild();
    void advance();
    bool isAtEnd();

    void close();

    static bool create(const char* path); // creates all parents
    static bool exists(const char* path);
    static bool destroy(const char* path); // recursive!
    static void print(const char* path);

private:
    Dir(const Dir&);
    Dir& operator=(const Dir&);

#if CK_PLATFORM_WIN || CK_PLATFORM_WP8
    HANDLE m_hFind;
    WIN32_FIND_DATA m_findData;
    bool m_valid;
#else
    DIR* m_dir;
    dirent* m_entry;
#endif

#if CK_PLATFORM_WP8
    char m_path[Path::k_maxLen];
#endif
};


}
