#pragma once

#include "ck/core/platform.h"
#include "ck/core/types.h"
#include "ck/core/binarystream.h"

namespace Cki
{


class MemoryStream : public BinaryStream
{
public:
    MemoryStream();
    MemoryStream(int capacity);
    ~MemoryStream();

    virtual int read(void* buf, int bytes);
    virtual int write(const void* buf, int bytes); // writing past end will grow the buffer
    virtual int getSize() const;
    virtual int getPos() const;
    virtual void setPos(int pos);
    virtual void close();

    void* getBuffer(); // careful!
    int getCapacity() const;
    void reserve(int capacity);
    void compact();

private:
    byte* m_buf;
    int m_bufSize;
    int m_size;
    int m_pos;
};


}

