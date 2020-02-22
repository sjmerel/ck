#include "ck/core/staticsingleton.h"
#include "ck/core/types.h"
#include <new>

namespace Cki
{


template <typename T>
void StaticSingleton<T>::init()
{
    if (!s_instance)
    {
        static byte mem[sizeof(T)];
        s_instance = new (mem) T;
    }
}

template <typename T>
void StaticSingleton<T>::shutdown()
{
    if (s_instance)
    {
        s_instance->~T();
        s_instance = NULL;
    }
}

template <typename T>
T* StaticSingleton<T>::s_instance = NULL;



}
