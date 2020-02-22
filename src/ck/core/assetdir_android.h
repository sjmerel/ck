#pragma once

#include "ck/core/platform.h"
#include "ck/core/types.h"

struct AAssetDir;

namespace Cki
{


class AssetDir
{
public:
    // Pass in null for top-level asset directory.
    // Note that this will not list sub-directories (could use libzip to do this, or just parse the zip header ourselves).
    AssetDir(const char* path = NULL); 
    ~AssetDir();

    bool isValid();

    const char* getChild();
    void advance();
    bool isAtEnd();

    void close();


private:
    AAssetDir* m_dir;
    const char* m_child;

    AssetDir(const AssetDir&);
    AssetDir& operator=(const AssetDir&);

};


}
