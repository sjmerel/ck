#pragma once

#include "ck/core/platform.h"
#include "ck/core/list.h"

namespace Cki
{


template <typename T, int N = 0>
class Listable : public List<T,N>::Node
{
protected:
    Listable();
    ~Listable();

    void destroy();

    static T* getFirst();
    static T* getLast();

private:
    static List<T,N> s_list;

    Listable(const Listable&);
    Listable& operator=(const Listable&);
};


}
