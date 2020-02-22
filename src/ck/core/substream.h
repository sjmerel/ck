#pragma once

#include "ck/core/platform.h"
#include "ck/core/binarystream.h"

namespace Cki
{


class SubStream : public BinaryStream
{
public:
    SubStream(BinaryStream&, int offset, int length);
    virtual ~SubStream() {}

    // BinaryStream overrides
    virtual bool isValid() const;
    virtual int read(void* buf, int bytes);
    virtual int write(const void* buf, int bytes);
    virtual int getSize() const;
    virtual int getPos() const;
    virtual void setPos(int pos);
    virtual void close();

private:
    BinaryStream& m_stream;
    int m_offset;
    int m_length;
    bool m_valid;
};


}
