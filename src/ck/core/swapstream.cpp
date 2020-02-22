#include "ck/core/swapstream.h"
#include "ck/core/mem.h"

namespace Cki
{


SwapStream::SwapStream(BinaryStream& stream, ByteOrder::Value byteOrder) :
    m_stream(stream),
    m_byteOrder(byteOrder)
{
}

void SwapStream::setByteOrder(ByteOrder::Value byteOrder)
{
    m_byteOrder = byteOrder;
}

ByteOrder::Value SwapStream::getByteOrder() const
{
    return m_byteOrder;
}

int SwapStream::read(void* buf, int bytes)
{
    // no swapping
    return m_stream.read(buf, bytes);
}

int SwapStream::write(const void* buf, int bytes)
{
    // no swapping
    return m_stream.write(buf, bytes);
}

int SwapStream::getSize() const
{
    return m_stream.getSize();
}

int SwapStream::getPos() const
{
    return m_stream.getPos();
}

void SwapStream::setPos(int pos)
{
    m_stream.setPos(pos);
}

void SwapStream::readValue(uint8& value)
{
    readImpl(value);
}

void SwapStream::readValue(int8& value)
{
    readImpl(value);
}

void SwapStream::readValue(uint16& value)
{
    readImpl(value);
}

void SwapStream::readValue(int16& value)
{
    readImpl(value);
}

void SwapStream::readValue(uint32& value)
{
    readImpl(value);
}

void SwapStream::readValue(int32& value)
{
    readImpl(value);
}

void SwapStream::readValue(uint64& value)
{
    readImpl(value);
}

void SwapStream::readValue(int64& value)
{
    readImpl(value);
}

void SwapStream::readValue(float& value)
{
    readImpl(value);
}

void SwapStream::readValue(double& value)
{
    readImpl(value);
}

void SwapStream::writeValue(uint8 value)
{
    writeImpl(value);
}

void SwapStream::writeValue(int8 value)
{
    writeImpl(value);
}

void SwapStream::writeValue(uint16 value)
{
    writeImpl(value);
}

void SwapStream::writeValue(int16 value)
{
    writeImpl(value);
}

void SwapStream::writeValue(uint32 value)
{
    writeImpl(value);
}

void SwapStream::writeValue(int32 value)
{
    writeImpl(value);
}

void SwapStream::writeValue(uint64 value)
{
    writeImpl(value);
}

void SwapStream::writeValue(int64 value)
{
    writeImpl(value);
}

void SwapStream::writeValue(float value)
{
    writeImpl(value);
}

void SwapStream::writeValue(double value)
{
    writeImpl(value);
}

////////////////////////////////////////

template <typename T> inline
void SwapStream::readImpl(T& value)
{
    m_stream.readValue(value);
    if (m_byteOrder != ByteOrder::k_native)
    {
        value = Mem::swap(value);
    }
}

template <typename T> inline
void SwapStream::writeImpl(T value)
{
    if (m_byteOrder != ByteOrder::k_native)
    {
        value = Mem::swap(value);
    }
    m_stream.writeValue(value);
}


}
