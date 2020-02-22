#include "ck/core/readstream.h"
#include "ck/core/path.h"
#include "ck/core/debug.h"
#include <new>

namespace Cki
{

CkCustomFileFunc ReadStream::s_handler = NULL;
void* ReadStream::s_handlerData = NULL;

ReadStream::ReadStream(const char* path, CkPathType pathType) :
    m_stream(NULL)
{
    if (s_handler)
    {
        Path fullPath(path, pathType);
        CkCustomFile* file = s_handler(fullPath.getBuffer(), s_handlerData);
        if (file)
        {
            m_stream = new (m_mem) CustomStream(file);
        }
    }

#if CK_PLATFORM_ANDROID
    if (!m_stream && pathType == kCkPathType_Asset)
    {
        m_stream = new (m_mem) AssetStream(path);
    }
#endif

    if (!m_stream)
    {
        Path fullPath(path, pathType);
        m_stream = new (m_mem) FileStream(fullPath.getBuffer(), FileStream::k_read);
    }
}

ReadStream::~ReadStream()
{
    m_stream->~BinaryStream();
}

bool ReadStream::isValid() const
{
    return m_stream->isValid();
}

int ReadStream::read(void* buf, int bytes)
{
    return m_stream->read(buf, bytes);
}

int ReadStream::write(const void* buf, int bytes)
{
    return 0; // read-only!
}

int ReadStream::getSize() const
{
    return m_stream->getSize();
}

int ReadStream::getPos() const
{
    return m_stream->getPos();
}

void ReadStream::setPos(int pos)
{
    m_stream->setPos(pos);
}

void ReadStream::close()
{
    m_stream->close();
}

bool ReadStream::exists(const char* path, CkPathType pathType)
{
    return getSize(path, pathType) >= 0;
}

int ReadStream::getSize(const char* path, CkPathType pathType)
{
    if (s_handler)
    {
        Path fullPath(path, pathType);
        CkCustomFile* file = s_handler(fullPath.getBuffer(), s_handlerData);
        if (file)
        {
            int size = file->getSize();
            delete file;
            return size;
        }
    }

#if CK_PLATFORM_ANDROID
    if (pathType == kCkPathType_Asset)
    {
        return AssetStream::getSize(path);
    }
#endif

    Path fullPath(path, pathType);
    return FileStream::getSize(fullPath.getBuffer());
}

void ReadStream::setFileHandler(CkCustomFileFunc handler, void* data)
{
    s_handler = handler;
    s_handlerData = data;
}


}
