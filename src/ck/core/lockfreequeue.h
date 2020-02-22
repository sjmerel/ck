#pragma once
#include "ck/core/platform.h"
#include "ck/core/pool.h"
#include <new>

#if CK_PLATFORM_WP8
#  include <atomic>
#endif

namespace Cki
{


// based on http://drdobbs.com/cpp/210604448

template <typename T>
class LockFreeQueue
{
public:
    LockFreeQueue(int size);
    ~LockFreeQueue();

    // call from producer thread only:
    bool produce(const T& t);
    void trim();

    // call from producer or consumer thread:
    bool consume(T& result);

private:
    struct Node 
    {
        Node(T val) : value(val), next(NULL) {}
        T value;
        Node* next;
    };

    Node* m_first;            // for producer only

    // Couldn't get explicit memory barrier to work on WP8, so using std::atomic;
    // However, std::atomic requires higher minimum deployment targets on iOS and OSX,
    // and requires linking with a specific C++ standard library on Android, which could
    // cause incompatibilities with apps that want to use a different one.
#if CK_PLATFORM_WP8
    std::atomic<Node*> m_divider; // shared
    std::atomic<Node*> m_last;    // shared
#else
    volatile Node* m_divider; // shared
    volatile Node* m_last;    // shared
#endif

    byte* m_buf;
    Pool m_pool;

};


}
