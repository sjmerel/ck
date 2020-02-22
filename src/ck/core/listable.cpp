#include "ck/core/listable.h"
#include "ck/core/types.h"

#include "ck/core/list.cpp" // template definition

namespace Cki
{


template <typename T, int N>
List<T,N> Listable<T,N>::s_list;

template <typename T, int N>
Listable<T,N>::Listable() :
    List<T,N>::Node()
{
    s_list.addFirst((T*) this);
}

template <typename T, int N>
Listable<T,N>::~Listable()
{
    s_list.remove((T*) this);
}

template <typename T, int N>
void Listable<T,N>::destroy()
{
    s_list.remove((T*) this);
}

template <typename T, int N>
T* Listable<T,N>::getFirst()
{
    return (T*) s_list.getFirst();
}

template <typename T, int N>
T* Listable<T,N>::getLast()
{
    return (T*) s_list.getLast();
}



}
