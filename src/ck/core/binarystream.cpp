#include "ck/core/binarystream.h"

namespace Cki
{


bool BinaryStream::isValid() const
{
    return true;
}

void BinaryStream::close()
{
}

void BinaryStream::readValue(uint8& value)
{
    readImpl(value);
}

void BinaryStream::readValue(int8& value)
{
    readImpl(value);
}

void BinaryStream::readValue(uint16& value)
{
    readImpl(value);
}

void BinaryStream::readValue(int16& value)
{
    readImpl(value);
}

void BinaryStream::readValue(uint32& value)
{
    readImpl(value);
}

void BinaryStream::readValue(int32& value)
{
    readImpl(value);
}

void BinaryStream::readValue(uint64& value)
{
    readImpl(value);
}

void BinaryStream::readValue(int64& value)
{
    readImpl(value);
}

void BinaryStream::readValue(float& value)
{
    readImpl(value);
}

void BinaryStream::readValue(double& value)
{
    readImpl(value);
}


void BinaryStream::writeValue(uint8 value)
{
    writeImpl(value);
}

void BinaryStream::writeValue(int8 value)
{
    writeImpl(value);
}

void BinaryStream::writeValue(uint16 value)
{
    writeImpl(value);
}

void BinaryStream::writeValue(int16 value)
{
    writeImpl(value);
}

void BinaryStream::writeValue(uint32 value)
{
    writeImpl(value);
}

void BinaryStream::writeValue(int32 value)
{
    writeImpl(value);
}

void BinaryStream::writeValue(uint64 value)
{
    writeImpl(value);
}

void BinaryStream::writeValue(int64 value)
{
    writeImpl(value);
}

void BinaryStream::writeValue(float value)
{
    writeImpl(value);
}

void BinaryStream::writeValue(double value)
{
    writeImpl(value);
}

BinaryStream& BinaryStream::operator<<(uint8 value)
{
    writeValue(value);
    return *this;
}

BinaryStream& BinaryStream::operator<<(int8 value)
{
    writeValue(value);
    return *this;
}

BinaryStream& BinaryStream::operator<<(uint16 value)
{
    writeValue(value);
    return *this;
}

BinaryStream& BinaryStream::operator<<(int16 value)
{
    writeValue(value);
    return *this;
}

BinaryStream& BinaryStream::operator<<(uint32 value)
{
    writeValue(value);
    return *this;
}

BinaryStream& BinaryStream::operator<<(int32 value)
{
    writeValue(value);
    return *this;
}

BinaryStream& BinaryStream::operator<<(uint64 value)
{
    writeValue(value);
    return *this;
}

BinaryStream& BinaryStream::operator<<(int64 value)
{
    writeValue(value);
    return *this;
}

BinaryStream& BinaryStream::operator<<(float value)
{
    writeValue(value);
    return *this;
}

BinaryStream& BinaryStream::operator>>(uint8& value)
{
    readValue(value);
    return *this;
}

BinaryStream& BinaryStream::operator>>(int8& value)
{
    readValue(value);
    return *this;
}

BinaryStream& BinaryStream::operator>>(uint16& value)
{
    readValue(value);
    return *this;
}

BinaryStream& BinaryStream::operator>>(int16& value)
{
    readValue(value);
    return *this;
}

BinaryStream& BinaryStream::operator>>(uint32& value)
{
    readValue(value);
    return *this;
}

BinaryStream& BinaryStream::operator>>(int32& value)
{
    readValue(value);
    return *this;
}

BinaryStream& BinaryStream::operator>>(uint64& value)
{
    readValue(value);
    return *this;
}

BinaryStream& BinaryStream::operator>>(int64& value)
{
    readValue(value);
    return *this;
}

BinaryStream& BinaryStream::operator>>(float& value)
{
    readValue(value);
    return *this;
}

////////////////////////////////////////

template <typename T> inline
void BinaryStream::readImpl(T& value)
{
    read(&value, sizeof(value));
}

template <typename T> inline
void BinaryStream::writeImpl(T value)
{
    write(&value, sizeof(value));
}



}
