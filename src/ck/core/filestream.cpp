#include "ck/core/filestream.h"
#include "ck/core/debug.h"
#include "ck/core/math.h"
#include "ck/core/path.h"
#include <sys/stat.h>

#if CK_PLATFORM_WIN || CK_PLATFORM_WP8
#  define unlink _unlink
#  define S_IFREG _S_IFREG
#else
#  include <unistd.h>
#endif

namespace Cki
{


FileStream::FileStream(const char* path, Mode mode) :
    m_file(NULL),
    m_size(-1),
    m_pos(0)
{
    const char* fmode = NULL;
    switch (mode)
    {
        case k_read: 
            fmode = "rb"; 
            break;

        case k_writeTruncate: 
            fmode = "wb"; 
            break;

        case k_readWrite: 
            fmode = "r+b"; 
            break;

        case k_readWriteTruncate: 
            fmode = "w+b"; 
            break;

        default:
            CK_FAIL("unknown mode");
    }

#if CK_PLATFORM_WIN
    wchar_t wpath[Path::k_maxLen];
    MultiByteToWideChar(CP_UTF8, 0, path, -1, wpath, Path::k_maxLen);
    wchar_t wfmode[12];
    MultiByteToWideChar(CP_UTF8, 0, fmode, -1, wfmode, 12);
    m_file = _wfopen(wpath, wfmode);
#else
    m_file = fopen(path, fmode);
#endif

    if (m_file)
    {
        if (mode == k_read || mode == k_readWrite)
        {
            m_size = getSize(path);
        }
        else
        {
            m_size = 0;
        }
    }
}

FileStream::~FileStream()
{
    close();
}

bool FileStream::isValid() const
{
    return m_file != NULL;
}

int FileStream::read(void* buf, int bytes)
{
    CK_ASSERT(buf);
    CK_ASSERT(bytes >= 0);

#if CK_PLATFORM_WIN || CK_PLATFORM_WP8
    // must call this when switching between read & write when opened in "r+" mode
    // TODO only do this when necessary
    CK_VERIFY(fseek(m_file, 0, SEEK_CUR) == 0);
#endif

    int bytesRead = (int) fread(buf, 1, bytes, m_file);
    m_pos += bytesRead;
    CK_ASSERT(m_pos <= m_size || bytesRead == 0);
    return bytesRead;
}

int FileStream::write(const void* buf, int bytes)
{
    CK_ASSERT(buf || bytes == 0);
    CK_ASSERT(bytes >= 0);

#if CK_PLATFORM_WIN || CK_PLATFORM_WP8
    // must call this when switching between read & write when opened in "r+" mode
    // TODO only do this when necessary
    CK_VERIFY(fseek(m_file, 0, SEEK_CUR) == 0);
#endif

    int bytesWritten = (int) fwrite(buf, 1, bytes, m_file);
    m_pos += bytesWritten;
    m_size = Math::max(m_size, m_pos);
    CK_ASSERT(m_pos <= m_size);
    return bytesWritten;
}

int FileStream::getSize() const
{
    return m_size;
}

void FileStream::setPos(int pos)
{
    CK_ASSERT(pos >= 0);
    CK_VERIFY(fseek(m_file, pos, SEEK_SET) == 0);
    CK_ASSERT(ftell(m_file) == pos);
    m_pos = pos;
}

int FileStream::getPos() const
{
    return m_pos;
}

void FileStream::close()
{
    if (m_file)
    {
        // if we extended the length of the stream, make sure the file is inflated
        CK_VERIFY(fseek(m_file, 0, SEEK_END) == 0);
        int pos = (int) ftell(m_file);
        CK_ASSERT(pos >= 0);
        if (m_size > pos)
        {
            CK_VERIFY(fseek(m_file, m_size-1, SEEK_SET) == 0);
            CK_VERIFY(fwrite("", 1, 1, m_file) == 1);
        }

        fclose(m_file);
        m_file = NULL;
    }
}

void FileStream::flush()
{
    CK_VERIFY(fflush(m_file) == 0);
}

////////////////////////////////////////

bool FileStream::exists(const char* path)
{
    return getSize(path) >= 0;
}

bool FileStream::destroy(const char* path)
{
    return unlink(path) == 0;
}

int FileStream::getSize(const char* path)
{
#if CK_PLATFORM_WIN
    struct _stat64i32 statBuf;
    wchar_t wpath[Path::k_maxLen];
    MultiByteToWideChar(CP_UTF8, 0, path, -1, wpath, Path::k_maxLen);
    if (_wstat(wpath, &statBuf) == 0)
#else
    struct stat statBuf;
    if (stat(path, &statBuf) == 0)
#endif
    {
        if (statBuf.st_mode & S_IFREG)
        {
            return (int) statBuf.st_size;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }
}



}
