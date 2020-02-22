#include "ck/core/path.h"
#include "ck/core/debug.h"
#include "ck/core/system_platform.h"

#if CK_PLATFORM_WIN || CK_PLATFORM_WP8
#  include <direct.h>
#  define getcwd _getcwd
#elif CK_PLATFORM_LINUX || CK_PLATFORM_ANDROID
#  include <unistd.h>
#endif


namespace
{
#if CK_PLATFORM_WIN || CK_PLATFORM_WP8
    const char k_sep = '\\';
    const char k_altSep = '/';
#else
    const char k_sep = '/';
    const char k_altSep = '\\';
#endif
}


namespace Cki
{

Path::Path() 
{
    canonicalize();
}

Path::Path(const char* path, CkPathType pathType)
{
    set(path, pathType);
    canonicalize();
}

#if !CK_PLATFORM_WP8
void Path::setCurrent()
{
    char buf[PathType::k_bufSize];
    getcwd(buf, PathType::k_bufSize);
    m_path = buf;
    canonicalize();
}
#endif

void Path::set(const char* path, CkPathType pathType)
{
    const char* dir = NULL;
    switch (pathType)
    {
        case kCkPathType_FileSystem:
            break;

#if CK_PLATFORM_ANDROID
        case kCkPathType_Asset:
            break;

        case kCkPathType_PrivateFiles:
            dir = SystemAndroid::get()->getFilesDir();
            break;

        case kCkPathType_ExternalStorage:
            dir = SystemAndroid::get()->getExternalStorageDir();
            break;
#endif

#if CK_PLATFORM_IOS || CK_PLATFORM_OSX || CK_PLATFORM_TVOS
        case kCkPathType_Resource:
            dir = SystemPlatform::get()->getResourceDir();
            break;
#endif

#if CK_PLATFORM_WP8
        case kCkPathType_LocalDir:
            dir = SystemWp8::get()->getLocalDir();
            break;

        case kCkPathType_InstallationDir:
            dir = SystemWp8::get()->getInstallationDir();
            break;
#endif

#if CK_PLATFORM_IOS || CK_PLATFORM_OSX || CK_PLATFORM_TVOS || CK_PLATFORM_WIN || CK_PLATFORM_LINUX
        case kCkPathType_Temp:
            dir = SystemPlatform::get()->getTempDir();
            break;
#endif

#if CK_PLATFORM_IOS || CK_PLATFORM_OSX || CK_PLATFORM_WIN || CK_PLATFORM_LINUX
        case kCkPathType_Documents:
            dir = SystemPlatform::get()->getDocumentsDir();
            break;
#endif

#if CK_PLATFORM_WIN || CK_PLATFORM_LINUX
        case kCkPathType_ExeDir:
            dir = SystemPlatform::get()->getExeDir();
            break;
#endif
#if CK_PLATFORM_OSX
        case kCkPathType_ExeDir:
            dir = SystemPlatform::get()->getResourceDir();
            break;
#endif

        default:
            CK_FAIL("illegal path type");
            break;
    }

    if (dir)
    {
        m_path = dir;
        appendChild(path);
    }
    else
    {
        m_path = path;
        canonicalize();
    }
}

void Path::append(const char* path)
{
    m_path.append(path);
    canonicalize();
}

void Path::appendChild(const char* path)
{
    m_path.append("/");
    m_path.append(path);
    canonicalize();
}

const char* Path::getBuffer() const
{
    return m_path.getBuffer();
}

int Path::getLength() const
{
    return m_path.getLength();
}

int Path::getDepth() const
{
    if (m_path.getLength() == 1 && (m_path[0] == '.' || m_path[0] == k_sep))
    {
        return 0;
    }
#if CK_PLATFORM_WIN || CK_PLATFORM_WP8
    if (m_path.getLength() == 3 && m_path[1] == ':' && m_path[2] == '\\')
    {
        // e.g. "c:\"
        return 0;
    }
#endif

    int seps = 0;
    for (int i = 0; i < m_path.getLength(); ++i)
    {
        if (m_path[i] == k_sep)
        {
            ++seps;
        }
    }

#if CK_PLATFORM_WIN || CK_PLATFORM_WP8
    if (m_path.startsWith("\\\\"))
    {
        // subtract 2 because "\\" is not a valid directory like "\" is;
        // "\\server" would be the root.
        return seps - 2;
    }
#endif
    if (isAbsolute())
    {
        // "/a/b" -> "/a" -> "/"
        return seps;
    }
    else
    {
        // "a/b" -> "a" -> "."
        return seps + 1;
    }
}

bool Path::isRelative() const
{
    return !isAbsolute();
}

bool Path::isAbsolute() const
{
    if (m_path.getLength() > 0 && m_path[0] == k_sep)
    {
        return true;
    }
#if CK_PLATFORM_WIN || CK_PLATFORM_WP8
    else if (m_path.getLength() >= 3 && m_path[1] == ':' && m_path[2] == '\\')
    {
        // e.g. "c:\"
        return true;
    }
#endif
    else
    {
        return false;
    }
}

void Path::setParent()
{
    CK_ASSERT(getDepth() > 0);
    for (int i = m_path.getLength() - 1; i >= 0; --i)
    {
        if (m_path[i] == k_sep)
        {
            if (i == 0)
            {
                // leading separator
                m_path.eraseFrom(i+1);
            }
#if CK_PLATFORM_WIN || CK_PLATFORM_WP8
            else if (i == 2 && m_path[1] == ':')
            {
                m_path.eraseFrom(i+1);
            }
#endif
            else
            {
                m_path.eraseFrom(i);
            }
            return;
        }
    }

    // there are no separators, so it's a relative path with depth 1
    m_path = ".";
}

const char* Path::getLeaf() const
{
    int sepPos = m_path.rfind(k_sep);
    if (sepPos >= 0)
    {
        return m_path.getBuffer() + sepPos + 1;
    }
    else
    {
        return m_path.getBuffer();
    }
}

const char* Path::getExtension() const
{
    int sepPos = m_path.rfind(k_sep);
    if (sepPos < 0)
    {
        sepPos = 0;
    }
    int dotPos = m_path.find('.', sepPos);
    if (dotPos >= 0)
    {
        return m_path.getBuffer() + dotPos + 1;
    }
    else
    {
        return NULL;
    }
}

bool Path::hasExtension(const char* ext) const
{
    String extStr((char*) getExtension(), String::External());
    return extStr.equals(ext, true);
}

void Path::setExtension(const char* ext)
{
    int sepPos = m_path.rfind(k_sep);
    if (sepPos < 0)
    {
        sepPos = 0;
    }
    int dotPos = m_path.find('.', sepPos);
    if (dotPos >= 0)
    {
        m_path.eraseFrom(dotPos);
    }

    if (ext)
    {
        append(".");
        append(ext);
    }
}


////////////////////////////////////////

void Path::canonicalize()
{
    // canonicalize separators
    //   /a\b -> /a/b
    int len = m_path.getLength();
    for (int i = 0; i < len; ++i)
    {
        if (m_path[i] == k_altSep)
        {
            m_path[i] = k_sep;
        }
    }

    // remove repeated separators
    //   //a///b -> /a/b
    len = m_path.getLength();
#if CK_PLATFORM_WIN || CK_PLATFORM_WP8
    // on windows, allow leading \\ for UNC paths
    for (int i = 2; i < len; ++i)
#else
    for (int i = 1; i < len; ++i)
#endif
    {
        if (m_path[i] == k_sep && m_path[i-1] == k_sep)
        {
            m_path.erase(i);
            --len;
            --i;
        }
    }

    // remove trailing separator
    //   /a/b/ -> /a/b
    len = m_path.getLength();
    if (len > 1 && m_path[len-1] == k_sep)
    {
        m_path.erase(len-1);
    }

    // replace "/./" with "/"
    //   /a/./b -> /a/b
    for (int i = 0; i < m_path.getLength() - 2; ++i)
    {
        if (m_path[i] == k_sep && m_path[i+1] == '.' && m_path[i+2] == k_sep)
        {
            m_path.erase(i, 2);
        }
    }

    // remove leading "./"
    //   ./a -> a
    if (m_path.getLength() >= 2)
    {
        if (m_path[0] == '.' && m_path[1] == k_sep)
        {
            m_path.erase(0,2);
        }
    }

    // remove trailing "/." 
    //   /a/. -> /a
    //   /. -> /
    len = m_path.getLength();
    if (len >= 2)
    {
        if (m_path[len-1] == '.' && m_path[len-2] == k_sep)
        {
            if (len > 2)
            {
                m_path.erase(len - 2, 2);
            }
            else
            {
                m_path.erase(len - 1);
            }
        }
    }

    // TODO replace "a/b/.." with "a"
}



}
