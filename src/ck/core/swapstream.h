#pragma once

#include "ck/core/platform.h"
#include "ck/core/types.h"
#include "ck/core/byteorder.h"
#include "ck/core/binarystream.h"

namespace Cki
{


class BinaryStream;

class SwapStream : public BinaryStream
{
public:
    SwapStream(BinaryStream&, ByteOrder::Value = ByteOrder::k_native);

    void setByteOrder(ByteOrder::Value);
    ByteOrder::Value getByteOrder() const;

    // these do not swap!
    virtual int read(void* buf, int bytes);
    virtual int write(const void* buf, int bytes);

    virtual int getSize() const;
    virtual int getPos() const;
    virtual void setPos(int pos);

    // these may swap byte order
    virtual void readValue(uint8&);
    virtual void readValue(int8&);
    virtual void readValue(uint16&);
    virtual void readValue(int16&);
    virtual void readValue(uint32&);
    virtual void readValue(int32&);
    virtual void readValue(uint64&);
    virtual void readValue(int64&);
    virtual void readValue(float&);
    virtual void readValue(double&);

    // these may swap byte order
    virtual void writeValue(uint8);
    virtual void writeValue(int8);
    virtual void writeValue(uint16);
    virtual void writeValue(int16);
    virtual void writeValue(uint32);
    virtual void writeValue(int32);
    virtual void writeValue(uint64);
    virtual void writeValue(int64);
    virtual void writeValue(float);
    virtual void writeValue(double);

private:
    BinaryStream& m_stream;
    ByteOrder::Value m_byteOrder;

    SwapStream(const SwapStream&);
    SwapStream& operator=(const SwapStream&);

    template <typename T> inline void readImpl(T& value);
    template <typename T> inline void writeImpl(T value);
};



}
