#pragma once

#include "ck/core/platform.h"
#include "ck/pathtype.h"
#include "ck/customfile.h"
#include "ck/core/binarystream.h"
#include "ck/core/filestream.h"
#include "ck/core/customstream.h"
#include "ck/core/math.h"
#if CK_PLATFORM_ANDROID
#  include "ck/core/assetstream_android.h"
#endif


namespace Cki
{


// read-only stream; reads files or assets
class ReadStream : public BinaryStream
{
public:
    ReadStream(const char* path, CkPathType = kCkPathType_Default);
    virtual ~ReadStream();

    // BinaryStream overrides
    virtual bool isValid() const;
    virtual int read(void* buf, int bytes);
    virtual int write(const void* buf, int bytes);
    virtual int getSize() const;
    virtual int getPos() const;
    virtual void setPos(int pos);
    virtual void close();

    static bool exists(const char* path, CkPathType = kCkPathType_Default);
    static int getSize(const char* path, CkPathType = kCkPathType_Default); // -1 if file does not exist

    static void setFileHandler(CkCustomFileFunc, void*);

private:
    BinaryStream* m_stream;
#if CK_PLATFORM_ANDROID
    enum { k_memSize = Math::Max3<sizeof(FileStream), sizeof(AssetStream), sizeof(CustomStream)>::value };
#else
    enum { k_memSize = Math::Max<sizeof(FileStream), sizeof(CustomStream)>::value };
#endif
    char m_mem[k_memSize];

    static CkCustomFileFunc s_handler;
    static void* s_handlerData;

    ReadStream(const ReadStream&);
    ReadStream& operator=(const ReadStream&);
};



}

