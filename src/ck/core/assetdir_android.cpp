#include "ck/core/assetdir_android.h"
#include "ck/core/system_android.h"
#include <stdio.h>
#include <android/asset_manager.h>

namespace Cki
{


AssetDir::AssetDir(const char* path) :
    m_dir(NULL),
    m_child(NULL)
{
    m_dir = AAssetManager_openDir(SystemAndroid::get()->getAssetManager(), path ? path : "");
    advance();
}

AssetDir::~AssetDir()
{
    close();
}

bool AssetDir::isValid()
{
    return m_dir;
}

const char* AssetDir::getChild()
{
    return m_child;
}

void AssetDir::advance()
{
    m_child = AAssetDir_getNextFileName(m_dir);
}

bool AssetDir::isAtEnd()
{
    return !m_child;
}

void AssetDir::close()
{
    if (m_dir)
    {
        AAssetDir_close(m_dir);
        m_dir = NULL;
        m_child = NULL;
    }
}


}
