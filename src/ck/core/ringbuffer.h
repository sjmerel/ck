#pragma once

#include "ck/core/platform.h"
#include "ck/core/types.h"
#include "ck/core/mutex.h"

namespace Cki
{


class RingBuffer
{
public:
    RingBuffer();
    RingBuffer(int bytes);
    ~RingBuffer();

    void init(int bytes);
    bool isInited() const;

    int getSize() const;
    int getStoredBytes() const;
    int getFreeBytes() const;

    int isEmpty() const;
    int isFull() const;

    // read and write can be called on separate threads
    int read(void* buf, int bytes);
    int write(const void* buf, int bytes);
    int consume(int bytes);

    void beginRead(int bytes, const void*& buf1, int& bytes1, const void*&buf2, int& bytes2);
    void endRead(int bytes);
    void beginWrite(int bytes, void*& buf1, int& bytes1, void*&buf2, int& bytes2);
    void endWrite(int bytes);

    void resetRead(); // call from read thread
    void resetWrite(); // call from write thread

private:
    byte* m_buf;
    int m_bufSize;
    int m_readPos;
    int m_writePos;
    int m_stored;
    bool m_reading;
    bool m_writing;
    Mutex m_storedMutex;

    RingBuffer(const RingBuffer&);
    RingBuffer& operator=(const RingBuffer&);
};


////////////////////////////////////////

inline 
bool RingBuffer::isInited() const 
{
    return m_buf != NULL; 
}

inline 
int RingBuffer::getSize() const 
{
    return m_bufSize; 
}

inline 
int RingBuffer::getStoredBytes() const 
{
    return m_stored; 
}

inline 
int RingBuffer::getFreeBytes() const 
{
    return m_bufSize - m_stored; 
}

inline 
int RingBuffer::isEmpty() const 
{
    return m_stored == 0; 
}

inline 
int RingBuffer::isFull() const 
{
    return m_stored == m_bufSize; 
}


}
