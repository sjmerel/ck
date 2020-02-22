#pragma once

#include "ck/core/platform.h"
#include "ck/core/callback.h"

namespace Cki
{


template <typename T>
class Proxied
{
protected:
    Proxied();
    ~Proxied();

    void destroy();

public:
    static void setProxyCallbacks(typename Callback1<T*>::Func createFunc, typename Callback1<T*>::Func destroyFunc, void* data);

private:
    static Callback1<T*> s_createCb;
    static Callback1<T*> s_destroyCb;
    bool m_destroyed;
};


}
