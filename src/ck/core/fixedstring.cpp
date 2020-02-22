#include "ck/core/fixedstring.h"

namespace Cki
{


template <int N>
FixedString<N>::FixedString() :
    String(m_array, N, External())
{
    clear();
}

template <int N>
FixedString<N>::FixedString(const char* s) :
    String(m_array, N, External())
{
    assign(s);
}

template <int N>
FixedString<N>::FixedString(const String& s) :
    String(m_array, N, External())
{
    assign(s);
}

template <int N>
FixedString<N>::FixedString(const FixedString<N>& s) :
    String(m_array, N, External())
{
    assign(s);
}

template <int N>
FixedString<N>& FixedString<N>::operator=(const FixedString<N>& s)
{
    if (this != &s)
    {
        assign(s);
    }
    return *this;
}


template <int N>
FixedString<N>::~FixedString() 
{
}


template class FixedString<32>;
template class FixedString<64>;
template class FixedString<128>;
template class FixedString<256>;
template class FixedString<384>;



}
