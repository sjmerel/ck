#pragma once

#include "ck/core/platform.h"

namespace Cki
{


template <typename T>
class StaticSingleton
{
public:
    static void init();
    static T* get() { return s_instance; }
    static void shutdown();

private:
    static T* s_instance;
};


}
