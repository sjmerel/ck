#pragma once

#include "ck/core/platform.h"
#include "ck/core/binarystream.h"
#include "ck/core/assetmanager_android.h"


namespace Cki
{


class AssetStream : public BinaryStream
{
public:
    AssetStream(const char* path);
    virtual ~AssetStream();

    // BinaryStream overrides
    virtual bool isValid() const;
    virtual int read(void* buf, int bytes);
    virtual int write(const void* buf, int bytes); // always returns 0!
    virtual int getSize() const;
    virtual void setPos(int pos);
    virtual int getPos() const;

//    // True if asset's buffer is allocated in ordinary RAM (as opposed to being memory-mapped)
//    bool isAllocated() const;

    void close();


    static bool exists(const char* path);
    static int getSize(const char* path); // -1 if file does not exist

private:
    AssetManager::AAsset* m_asset;
    int m_size;
    int m_pos;
    byte m_assetBuf[AssetManager::k_assetSize];

    AssetStream(const AssetStream&);
    AssetStream& operator=(const AssetStream&);
};



}

