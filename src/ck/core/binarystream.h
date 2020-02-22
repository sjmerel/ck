#pragma once

#include "ck/core/platform.h"
#include "ck/core/allocatable.h"
#include "ck/core/types.h"

namespace Cki
{


class BinaryStream : public Allocatable
{
public:
    BinaryStream() {}
    virtual ~BinaryStream() {}

    // Returns false if the stream is not valid (e.g. file doesn't exist)
    virtual bool isValid() const;

    // Returns number of bytes actually read.  
    // Will not read past end of stream.
    virtual int read(void* buf, int bytes) = 0;

    // Returns number of bytes actually written.  
    // Writing past the end of the stream may increase the size.
    // Use write(NULL, 0) to increase size without actually writing.
    virtual int write(const void* buf, int bytes) = 0;

    // Current size of the stream
    virtual int getSize() const = 0;

    // Current position in the stream
    virtual int getPos() const = 0;

    // Set the current position in the stream.
    // If stream is growable, can set position past the end; the size will increase when you write past the end.
    virtual void setPos(int pos) = 0;

    // Close resources associated with this stream
    virtual void close();

    // Convenience to test whether position is at the end
    bool isAtEnd() const;

    // Convenience to skip forward
    void skip(int bytes);


    // Read primitive types
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

    // Write primitive types
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


    BinaryStream& operator<<(uint8);
    BinaryStream& operator<<(int8);
    BinaryStream& operator<<(uint16);
    BinaryStream& operator<<(int16);
    BinaryStream& operator<<(uint32);
    BinaryStream& operator<<(int32);
    BinaryStream& operator<<(uint64);
    BinaryStream& operator<<(int64);
    BinaryStream& operator<<(float);

    BinaryStream& operator>>(uint8&);
    BinaryStream& operator>>(int8&);
    BinaryStream& operator>>(uint16&);
    BinaryStream& operator>>(int16&);
    BinaryStream& operator>>(uint32&);
    BinaryStream& operator>>(int32&);
    BinaryStream& operator>>(uint64&);
    BinaryStream& operator>>(int64&);
    BinaryStream& operator>>(float&);


    // for new serializable types, define these:
    // void T::write(BinaryStream&) const;
    // void T::read(BinaryStream&);

    template <typename T>
    inline
    void readValue(T& value)
    {
        value.read(*this);
    }

    template <typename T>
    inline
    void writeValue(const T& value)
    {
        value.write(*this);
    }

    template <typename T>
    inline
    BinaryStream& operator<<(const T& value)
    {
        value.write(*this);
        return *this;
    }

    template <typename T>
    inline
    BinaryStream& operator>>(T& value)
    {
        value.read(*this);
        return *this;
    }

private:
    BinaryStream(const BinaryStream&);
    BinaryStream& operator=(const BinaryStream&);

    template <typename T> inline void readImpl(T&);
    template <typename T> inline void writeImpl(T);
};


////////////////////////////////////////

inline
bool BinaryStream::isAtEnd() const 
{ 
    return getPos() >= getSize(); 
}

inline 
void BinaryStream::skip(int bytes) 
{ 
    setPos(getPos() + bytes); 
}


}

