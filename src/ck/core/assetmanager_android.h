#pragma once

#include "ck/core/platform.h"
#include <jni.h>
#include <sys/types.h>

namespace Cki
{


namespace AssetManager
{
    void init(jobject activity);
    void shutdown();

    struct AAsset;
#if CK_32_BIT
    enum { k_assetSize = 16 };
#else
    enum { k_assetSize = 24 };
#endif

    enum 
    {
        AASSET_MODE_UNKNOWN      = 0,
        AASSET_MODE_RANDOM       = 1,
        AASSET_MODE_STREAMING    = 2,
        AASSET_MODE_BUFFER       = 3
    };

    AAsset* open(const char* filename, int mode, void* assetBuf);
    int read(AAsset*, void* buf, size_t count);
    off_t seek(AAsset*, off_t offset);
    off_t getLength(AAsset*);
    int openFileDescriptor(AAsset*, off_t* start, off_t* length);
    void close(AAsset*);
}


}
