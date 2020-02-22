#include "ck/core/proxied.h"

namespace Cki
{

template <typename T>
Proxied<T>::Proxied() :
    m_destroyed(false)
{
    s_createCb.call((T*) this);
}

template <typename T>
Proxied<T>::~Proxied()
{
    destroy();
}

template <typename T>
void Proxied<T>::destroy()
{
    if (!m_destroyed)
    {
        s_destroyCb.call((T*) this);
        m_destroyed = true;
    }
}

template <typename T>
void Proxied<T>::setProxyCallbacks(typename Callback1<T*>::Func createFunc, typename Callback1<T*>::Func destroyFunc, void* data)
{
    s_createCb.set(createFunc, data);
    s_destroyCb.set(destroyFunc, data);
}

template <typename T>
Callback1<T*> Proxied<T>::s_createCb;

template <typename T>
Callback1<T*> Proxied<T>::s_destroyCb;


}
