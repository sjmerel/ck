#pragma once

#include "ck/core/platform.h"
#include "ck/core/types.h"
#include "ck/core/binarystream.h"

namespace Cki
{


//
// Binary stream backed by external buffer
//

class BufferStream : public BinaryStream
{
public:
    BufferStream(void* buf, int bufSize, int size = 0);

    virtual int read(void* buf, int bytes);
    virtual int write(const void* buf, int bytes); // will not grow buffer!
    virtual int getSize() const;
    virtual int getPos() const;
    virtual void setPos(int pos);

    int getCapacity() const;

private:
    byte* m_buf;
    int m_bufSize; // size of buffer; may be > size of stream
    int m_size; // size of stream
    int m_pos;
};


}
