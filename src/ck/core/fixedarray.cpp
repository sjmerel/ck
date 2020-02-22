#include "ck/core/fixedarray.h"

namespace Cki
{


template <typename T, int N>
FixedArray<T,N>::FixedArray() :
    Array<T>((T*) m_buf, N, Array<T>::External)
{
}

template <typename T, int N>
FixedArray<T,N>::FixedArray(const Array<T>& array) :
    Array<T>((T*) m_buf, N, Array<T>::External)
{
    Array<T>::assign(array);
}

template <typename T, int N>
FixedArray<T,N>::FixedArray(const T* buf, int size) :
    Array<T>((T*) m_buf, N, Array<T>::External)
{
    Array<T>::assign(buf, size);
}

template <typename T, int N>
FixedArray<T,N>::FixedArray(const FixedArray<T,N>& array) :
    Array<T>((T*) m_buf, N, Array<T>::External)
{
    Array<T>::assign(array);
}

template <typename T, int N>
FixedArray<T,N>& FixedArray<T,N>::operator=(const FixedArray<T,N>& array)
{
    if (&array != this)
    {
        Array<T>::assign(array);
    }
    return *this;
}

template <typename T, int N>
FixedArray<T,N>::~FixedArray()
{
}


}
