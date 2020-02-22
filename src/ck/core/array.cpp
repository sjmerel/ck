#include "ck/core/array.h"
#include "ck/core/math.h"
#include "ck/core/debug.h"
#include "ck/core/mem.h"
#include "ck/core/binarystream.h"
#include "ck/core/memoryfixup.h"
#include <memory.h>

namespace Cki
{



template <typename T>
Array<T>::Array() :
    m_buf(NULL),
#if CK_32_BIT
    m_pad(0),
#endif
    m_size(0),
    m_capacity(0),
    m_locked(false),
    m_external(false)
{
}

template <typename T>
Array<T>::Array(const Array<T>& array) :
    m_buf(NULL),
#if CK_32_BIT
    m_pad(0),
#endif
    m_size(0),
    m_capacity(0),
    m_locked(false),
    m_external(false)
{
    assign(array);
}

template <typename T>
Array<T>::Array(const T* array, int size) :
    m_buf(NULL),
#if CK_32_BIT
    m_pad(0),
#endif
    m_size(0),
    m_capacity(0),
    m_locked(false),
    m_external(false)
{
    assign(array, size);
}

template <typename T>
Array<T>::Array(T* buf, int size, ExternalTag) :
    m_buf(buf),
#if CK_32_BIT
    m_pad(0),
#endif
    m_size(0),
    m_capacity(size),
    m_locked(false),
    m_external(true)
{
}

template <typename T>
Array<T>& Array<T>::operator=(const Array<T>& array)
{
    if (this != &array)
    {
        assign(array);
    }
    return *this;
}

template <typename T>
Array<T>::~Array()
{
    for (int i = 0; i < m_size; ++i)
    {
        m_buf[i].~T(); // call dtors
    }
    if (!m_external)
    {
        Mem::free(m_buf);
    }
}

template <typename T>
void Array<T>::setSize(int size, const T& fillValue)
{
    if (size > m_size)
    {
        reserve(size);
        for (int i = m_size; i < size; ++i)
        {
            new(m_buf + i) T(fillValue);
        }
    }
    else if (size < m_size)
    {
        for (int i = size; i < m_size; ++i)
        {
            m_buf[i].~T();
        }
    }
    m_size = size;
}

template <typename T>
void Array<T>::clear()
{
    m_size = 0;
}

template <typename T>
void Array<T>::reserve(int capacity)
{
    if (m_external)
    {
        if (capacity > m_capacity)
        {
            CK_FAIL("array buffer too small for requested capacity!");
        }
    }
    else
    {
        if (capacity > m_capacity)
        {
            CK_ASSERT(!m_locked);
            T* newBuf = (T*) Mem::alloc(capacity * sizeof(T));
            if (m_buf)
            {
                CK_ASSERT(m_capacity >= m_size);
                Mem::copy(newBuf, m_buf, m_size * sizeof(T));
                Mem::free(m_buf);
            }
            m_buf = newBuf;
            m_capacity = capacity;
        }
    }
}

template <typename T>
int Array<T>::getCapacity() const
{
    return m_capacity;
}

template <typename T>
void Array<T>::setLocked(bool locked)
{
    m_locked = locked;
}

template <typename T>
void Array<T>::compact()
{
    if (!m_external)
    {
        if (m_capacity > m_size)
        {
            T* newBuf = NULL;
            if (m_size > 0)
            {
                newBuf = (T*) Mem::alloc(m_size * sizeof(T));
                Mem::copy(newBuf, m_buf, m_size * sizeof(T));
            }
            Mem::free(m_buf);
            m_buf = newBuf;
            m_capacity = m_size;
        }
    }
}

template <typename T>
void Array<T>::assign(const Array<T>& array)
{
    assignImpl(array.m_buf, array.m_size);
}

template <typename T>
void Array<T>::assign(const T* array, int size)
{
    if (!array)
    {
        clear();
    }
    else
    {
        assignImpl(array, size);
    }
}

template <typename T>
void Array<T>::append(const T& t)
{
    appendImpl(&t, 1);
}

template <typename T>
void Array<T>::append(const Array<T>& array)
{
    appendImpl(array.m_buf, array.m_size);
}

template <typename T>
void Array<T>::append(const T* array, int size)
{
    appendImpl(array, size);
}

template <typename T>
const T& Array<T>::operator[](int i) const
{
    CK_ASSERT(i >= 0 && i < m_size);
    return m_buf[i];
}

template <typename T>
T& Array<T>::operator[](int i)
{
    CK_ASSERT(i >= 0 && i < m_size);
    return m_buf[i];
}

template <typename T>
const T& Array<T>::first() const
{
    CK_ASSERT(m_size > 0);
    return m_buf[0];
}

template <typename T>
T& Array<T>::first()
{
    CK_ASSERT(m_size > 0);
    return m_buf[0];
}

template <typename T>
const T& Array<T>::last() const
{
    CK_ASSERT(m_size > 0);
    return m_buf[m_size-1];
}

template <typename T>
T& Array<T>::last()
{
    CK_ASSERT(m_size > 0);
    return m_buf[m_size-1];
}

template <typename T>
const T* Array<T>::getBuffer() const
{
    return m_buf;
}

template <typename T>
T* Array<T>::getBuffer()
{
    return m_buf;
}

template <typename T>
void Array<T>::erase(int pos, int n )
{
    CK_ASSERT(pos >= 0 && pos < m_size);
    CK_ASSERT(pos + n <= m_size);
    CK_ASSERT(n >= 0);
    if (n > 0)
    {
        int newSize = m_size - n;
        for (int i = pos; i < newSize; ++i)
        {
            m_buf[i].~T();
            m_buf[i] = m_buf[i+n];
        }
        m_size = newSize;
        CK_ASSERT(m_size >= 0);
    }
}

template <typename T>
void Array<T>::insert(int pos, const T& value)
{
    insertImpl(pos, &value, 1);
}

template <typename T>
void Array<T>::insert(int pos, const Array<T>& array)
{
    insertImpl(pos, array.m_buf, array.m_size);
}


template <typename T>
void Array<T>::insert(int pos, const T* array, int size)
{
    insertImpl(pos, array, size);
}

template <typename T>
void Array<T>::write(BinaryStream& stream) const
{
    stream << (uint64) 0; // m_buf
    stream << m_size;  // m_size
    stream << m_size;  // m_capacity 
    stream << (bool32) true; // m_external
    stream << (bool32) true; // m_locked

    int pos = stream.getPos();
    int dataPos = stream.getSize();
    stream.setPos(dataPos + (m_size * sizeof(T)));
    stream.write(NULL, 0); // set size
    CK_ASSERT(stream.getSize() == stream.getPos());
    stream.setPos(dataPos);
    for (int i = 0; i < m_size; ++i)
    {
        stream << m_buf[i];
    }
    stream.setPos(pos);
}

template <typename T>
void Array<T>::fixup(MemoryFixup& fixup)
{
    m_buf = (T*) fixup.getPointer();
    fixup.advance(m_size * sizeof(T));
    for (int i = 0; i < m_size; ++i)
    {
        fixup.fixup(m_buf[i]);
    }
}

////////////////////////////////////////

template <typename T>
void Array<T>::grow(int capacity)
{
    if (capacity > m_capacity)
    {
        // don't grow by less than 100%.
        reserve(Math::max(capacity, m_capacity * 2));
    }
}

template <typename T>
void Array<T>::assignImpl(const T* array, int size)
{
    clear();
    appendImpl(array, size);
}

template <typename T>
void Array<T>::appendImpl(const T* array, int size)
{
    if (array)
    {
        grow(m_size + size);
        int elemsToCopy = Math::min(size, m_capacity - m_size);
        CK_ASSERT(elemsToCopy >= 0);
        for (int i = 0; i < elemsToCopy; ++i)
        {
            new(m_buf + m_size + i) T(array[i]); // in-place copy ctor
        }
        m_size += elemsToCopy;
        CK_ASSERT(m_size <= m_capacity);
    }
}

template <typename T>
void Array<T>::insertImpl(int pos, const T* array, int size)
{
    if (array)
    {
        CK_ASSERT(pos >= 0 && pos <= m_size);
        reserve(m_size + size);

        // move values to make space for insertion
        int newSize = Math::min(m_size + size, m_capacity);
        for (int i = newSize - 1; i >= pos + size; --i)
        {
            m_buf[i] = m_buf[i-size];
        }

        // write inserted values
        int elemsToInsert = Math::min(size, m_capacity - pos);
        for (int i = 0; i < elemsToInsert; ++i)
        {
            new(m_buf + pos + i) T(array[i]);
        }

        m_size = newSize;
    }
}

}

