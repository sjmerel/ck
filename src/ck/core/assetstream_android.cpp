#include "ck/core/assetstream_android.h"
#include "ck/core/debug.h"
#include "ck/core/math.h"
#include "ck/core/system_android.h"
#include <stdio.h>


namespace Cki
{


AssetStream::AssetStream(const char* path) :
    m_asset(NULL),
    m_size(-1),
    m_pos(0)
{
    // TODO other modes?
    m_asset = AssetManager::open(path, AssetManager::AASSET_MODE_STREAMING, m_assetBuf);

    if (m_asset)
    {
        m_size = AssetManager::getLength(m_asset);
    }
}

AssetStream::~AssetStream()
{
    close();
}

bool AssetStream::isValid() const
{
    return m_asset;
}

int AssetStream::read(void* buf, int bytes)
{
    CK_ASSERT(buf);
    CK_ASSERT(bytes >= 0);

    int bytesRead = AssetManager::read(m_asset, buf, bytes);
    if (bytesRead < 0)
    {
        return 0;
    }
    m_pos += bytesRead;
    CK_ASSERT(m_pos <= m_size || bytesRead == 0);
    return bytesRead;
}

int AssetStream::write(const void* buf, int bytes)
{
    CK_ASSERT(buf);
    CK_ASSERT(bytes >= 0);

    // read-only!
    return 0;
}

int AssetStream::getSize() const
{
    return m_size;
}

void AssetStream::setPos(int pos)
{
    CK_ASSERT(pos >= 0);
    AssetManager::seek(m_asset, Math::clamp(pos, 0, m_size));
    m_pos = pos;
}

int AssetStream::getPos() const
{
    return m_pos;
}

//bool AssetStream::isAllocated() const
//{
//    return AssetManager::isAllocated(m_asset);
//}

void AssetStream::close()
{
    if (m_asset)
    {
        AssetManager::close(m_asset);
        m_asset = NULL;
    }
}

////////////////////////////////////////

bool AssetStream::exists(const char* path)
{
    return getSize(path) >= 0;
}

int AssetStream::getSize(const char* path)
{
    AssetStream s(path);
    if (s.isValid())
    {
        return s.getSize();
    }
    else
    {
        return -1;
    }
}



}
