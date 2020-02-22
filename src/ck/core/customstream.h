#pragma once

#include "ck/customfile.h"
#include "ck/core/platform.h"
#include "ck/core/types.h"
#include "ck/core/binarystream.h"

namespace Cki
{


class CustomStream : public BinaryStream
{
public:
    CustomStream(CkCustomFile*);
    virtual ~CustomStream();

    virtual bool isValid() const;
    virtual int read(void* buf, int bytes);
    virtual int write(const void* buf, int bytes);
    virtual int getSize() const;
    virtual int getPos() const;
    virtual void setPos(int pos);
    virtual void close();

private:
    CkCustomFile* m_file;
};


}


