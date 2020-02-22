#pragma once

#include "ck/core/platform.h"
#include "ck/core/types.h"

namespace Cki
{

class BinaryStream;
class MemoryFixup;

template <typename T>
class Array
{
public:
    Array();
    Array(const Array&);
    Array(const T*, int size);

    enum ExternalTag { External };
    Array(T* buf, int size, ExternalTag); // uses external storage

    Array& operator=(const Array&);

    ~Array();

    int getSize() const { return m_size; }
    void setSize(int, const T& fillValue = T());
    bool isEmpty() const { return m_size == 0; }
    void clear();

    // When using external storage, capacity may not be increased to the requested amount.
    void reserve(int capacity);
    int getCapacity() const;
    void setLocked(bool); // if locked, asserts if capacity is increased
    void compact();

    void assign(const Array<T>&);
    void assign(const T*,  int size);

    void append(const T&);
    void append(const Array<T>&);
    void append(const T*,  int size);

    inline const T& operator[](int) const;
    inline T& operator[](int);

    inline const T& first() const;
    inline T& first();
    inline const T& last() const;
    inline T& last();

    inline const T* getBuffer() const;
    inline T* getBuffer();

    void erase(int pos, int n = 1);

    void insert(int pos, const T&);
    void insert(int pos, const Array<T>&);
    void insert(int pos, const T*, int size);

    // memory image
    void write(BinaryStream&) const;
    void fixup(MemoryFixup&);

private:
    // NOTE: size of structure, and offset of each member, must be same on 32-bit and 64-bit archs!
    T* m_buf; 
#if CK_32_BIT
    uint32 m_pad;
#endif
    int32 m_size;
    int32 m_capacity;
    bool32 m_locked; // capacity locked
    bool32 m_external; // external storage

    void grow(int capacity);
    void assignImpl(const T* array, int size);
    void appendImpl(const T* array, int size);
    void insertImpl(int pos, const T* array, int size);
};


}
