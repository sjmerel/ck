#pragma once

#include "ck/core/platform.h"
#include "ck/core/types.h"

namespace Cki
{



template <typename FUNC>
class CallbackBase
{
public:
    void set(FUNC func, void* data)
    {
        m_func = func;
        m_data = data;
    }

    void get(FUNC& func, void*& data)
    {
        func = m_func;
        data = m_data;
    }

protected:
    FUNC m_func;
    void* m_data;

    CallbackBase() : m_func(NULL), m_data(NULL) {}
};

struct Func0
{
    typedef void (*Func)(void*);
};

template <typename T>
struct Func1
{
    typedef void (*Func)(T, void*);
};

template <typename T, typename U>
struct Func2
{
    typedef void (*Func)(T, U, void*);
};

template <typename T, typename U, typename V>
struct Func3
{
    typedef void (*Func)(T, U, V, void*);
};

template <typename T, typename U, typename V, typename W>
struct Func4
{
    typedef void (*Func)(T, U, V, W, void*);
};

////////////////////////////////////////

class Callback0 : public CallbackBase<Func0::Func>
{
public:
    typedef Func0::Func Func;

    void call()
    {
        if (BaseType::m_func)
        {
            BaseType::m_func(BaseType::m_data);
        }
    }

private:
    typedef CallbackBase<Func0::Func> BaseType;
};

////////////////////////////////////////

template <typename T>
class Callback1 : public CallbackBase< typename Func1<T>::Func >
{
public:
    typedef typename Func1<T>::Func Func;

    void call(T t)
    {
        if (BaseType::m_func)
        {
            BaseType::m_func(t, BaseType::m_data);
        }
    }

private:
    typedef CallbackBase< typename Func1<T>::Func > BaseType;
};

////////////////////////////////////////

template <typename T, typename U>
class Callback2 : public CallbackBase< typename Func2<T,U>::Func >
{
public:
    typedef typename Func2<T,U>::Func Func;

    void call(T t, U u)
    {
        if (BaseType::m_func)
        {
            BaseType::m_func(t, u, BaseType::m_data);
        }
    }

private:
    typedef CallbackBase< typename Func2<T,U>::Func > BaseType;
};

////////////////////////////////////////

template <typename T, typename U, typename V>
class Callback3 : public CallbackBase< typename Func3<T,U, V>::Func >
{
public:
    typedef typename Func3<T,U,V>::Func Func;

    void call(T t, U u, V v)
    {
        if (BaseType::m_func)
        {
            BaseType::m_func(t, u, v, BaseType::m_data);
        }
    }

private:
    typedef CallbackBase< typename Func3<T,U,V>::Func > BaseType;
};

////////////////////////////////////////

template <typename T, typename U, typename V, typename W>
class Callback4 : public CallbackBase< typename Func4<T,U,V,W>::Func >
{
public:
    typedef typename Func4<T,U,V,W>::Func Func;

    void call(T t, U u, V v, W w)
    {
        if (BaseType::m_func)
        {
            BaseType::m_func(t, u, v, w, BaseType::m_data);
        }
    }

private:
    typedef CallbackBase< typename Func4<T,U,V,W>::Func > BaseType;
};


}
