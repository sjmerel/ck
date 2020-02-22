#pragma once

#include "ck/core/platform.h"
#include "ck/core/array.h"

namespace Cki
{


template <typename T, int N>
class FixedArray : public Array<T>
{
public:
    FixedArray();
    FixedArray(const Array<T>&);
    FixedArray(const T*, int size);

    FixedArray(const FixedArray<T,N>&);
    FixedArray& operator=(const FixedArray<T,N>&);

    ~FixedArray();

private:
    byte m_buf[N * sizeof(T)];

};


}
