#include "ck/core/dir.h"
#include "ck/core/debug.h"
#include "ck/core/string.h"
#include "ck/core/filestream.h"
#include <sys/stat.h>
#include <stdio.h>

#if !CK_PLATFORM_WIN && !CK_PLATFORM_WP8
#include <unistd.h>
#endif

namespace Cki
{

#if CK_PLATFORM_WIN || CK_PLATFORM_WP8
Dir::Dir(const char* pathname) 
{
    Path path(pathname);
    path.append("\\*.*");
#if CK_PLATFORM_WIN 
    m_hFind = FindFirstFile(path.getBuffer(), &m_findData);
#elif CK_PLATFORM_WP8
    wchar_t wpath[Path::k_maxLen];
    size_t n = mbstowcs(wpath, path.getBuffer(), Path::k_maxLen);
    if (n < 0 || n == Path::k_maxLen)
    {
        CK_FAIL("could not convert path");
        m_hFind = INVALID_HANDLE_VALUE;
    }
    else
    {
        m_hFind = FindFirstFileEx(wpath,
                FindExInfoStandard,
                &m_findData,
                FindExSearchNameMatch,
                NULL,
                0);
    }
#endif 
    m_valid = (m_hFind != INVALID_HANDLE_VALUE);
    while (m_hFind != INVALID_HANDLE_VALUE && (String::equals(getChild(), ".") || String::equals(getChild(), "..")))
    {
        advance();
    }
}

Dir::~Dir()
{
    close();
}

bool Dir::isValid()
{
    return m_valid;
}

const char* Dir::getChild()
{
    if (m_hFind == INVALID_HANDLE_VALUE)
    {
        return NULL;
    }
    else
    {
#if CK_PLATFORM_WIN
        return m_findData.cFileName;
#elif CK_PLATFORM_WP8
        size_t n = wcstombs(m_path, m_findData.cFileName, Path::k_maxLen);
        if (n < 0 || n == Path::k_maxLen)
        {
            CK_FAIL("could not convert path");
            return NULL;
        }
        else
        {
            return m_path;
        }
#endif
    }
}

void Dir::advance()
{
    if (!FindNextFile(m_hFind, &m_findData))
    {
        FindClose(m_hFind);
        m_hFind = INVALID_HANDLE_VALUE;
    }
}

bool Dir::isAtEnd()
{
    return m_hFind == INVALID_HANDLE_VALUE;
}

void Dir::close()
{
    if (m_hFind != INVALID_HANDLE_VALUE)
    {
        FindClose(m_hFind);
        m_hFind = INVALID_HANDLE_VALUE;
    }
}

#else

Dir::Dir(const char* pathname) :
    m_dir(NULL),
    m_entry(NULL)
{
    Path path(pathname);
    m_dir = opendir(path.getBuffer());
    if (m_dir)
    {
        advance();
    }
}

Dir::~Dir()
{
    close();
}

bool Dir::isValid()
{
    return m_dir;
}

const char* Dir::getChild()
{
    CK_ASSERT(m_dir);
    if (m_entry)
    {
        return m_entry->d_name;
    }
    else
    {
        return NULL;
    }
}

void Dir::advance()
{
    CK_ASSERT(m_dir);
    do
    {
        m_entry = readdir(m_dir);
    }
    while (m_entry && (String::equals(getChild(), ".") || String::equals(getChild(), "..")));
}

bool Dir::isAtEnd()
{
    CK_ASSERT(m_dir);
    return !m_entry;
}

void Dir::close()
{
    if (m_dir)
    {
        closedir(m_dir);
        m_dir = NULL;
    }
}
#endif

////////////////////////////////////////

bool Dir::create(const char* path)
{
    // create parents too
    Path base(path);
    int depth = base.getDepth();
    
    for (int i = 0; i <= depth; ++i)
    {
        Path parent = base;
        for (int j = 0; j < depth - i; ++j)
        {
            parent.setParent();
        }
        if (!exists(parent.getBuffer()))
        {
#if CK_PLATFORM_WIN
            CreateDirectory(parent.getBuffer(), NULL);
#elif CK_PLATFORM_WP8
            wchar_t wpath[Path::k_maxLen];
            size_t n = mbstowcs(wpath, parent.getBuffer(), Path::k_maxLen);
            if (n < 0 || n == Path::k_maxLen)
            {
                CK_FAIL("could not convert path");
                return false;
            }
            else
            {
                CreateDirectory(wpath, NULL);
            }
#else
            mkdir(parent.getBuffer(), 0777);
#endif
        }
    }

    return exists(path);
}

bool Dir::exists(const char* path)
{
#if CK_PLATFORM_WIN
    DWORD attr = GetFileAttributes(path);
    if (attr == INVALID_FILE_ATTRIBUTES)
    {
        return false;
    }
    else
    {
        return (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
    }
#elif CK_PLATFORM_WP8
    wchar_t wpath[Path::k_maxLen];
    size_t n = mbstowcs(wpath, path, Path::k_maxLen);
    if (n < 0 || n == Path::k_maxLen)
    {
        CK_FAIL("could not convert path");
        return false;
    }

    WIN32_FILE_ATTRIBUTE_DATA attr;
    if (GetFileAttributesEx(wpath, GetFileExInfoStandard, &attr))
    {
        return (attr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    }
    else
    {
        return false;
    }
#else
    struct stat statBuf;
    if (stat(path, &statBuf) == 0)
    {
        return S_ISDIR(statBuf.st_mode);
    }
    else
    {
        return false;
    }
#endif
}

bool Dir::destroy(const char* path)
{
    // recursively delete all
    if (Dir::exists(path))
    {
        Dir dir(path);
        while (!dir.isAtEnd())
        {
            // TODO FixedString?
            String child;
            child.printf("%s/%s", path, dir.getChild());
            if (Dir::exists(child.getBuffer()))
            {
                // subdir
                Dir::destroy(child.getBuffer());
            }
            else
            {
                FileStream::destroy(child.getBuffer());
            }

            dir.advance();
        }
#if CK_PLATFORM_WIN
        return RemoveDirectory(path) == TRUE;
#elif CK_PLATFORM_WP8
        wchar_t wpath[Path::k_maxLen];
        size_t n = mbstowcs(wpath, path, Path::k_maxLen);
        if (n < 0 || n == Path::k_maxLen)
        {
            CK_FAIL("could not convert path");
            return false;
        }
        else
        {
            return RemoveDirectory(wpath) == TRUE;
        }
#else
        return rmdir(path) == 0;
#endif
    }
    else
    {
        return false;
    }
}

void Dir::print(const char* path)
{
    Dir dir(path);
    if (dir.isValid())
    {
        while (!dir.isAtEnd())
        {
            CK_PRINT("%s/%s\n", path, dir.getChild());

            String child(path);
            child.append("/");
            child.append(dir.getChild());
            if (Dir::exists(child.getBuffer()))
            {
                Dir::print(child.getBuffer());
            }

            dir.advance();
        }
    }
}


}
