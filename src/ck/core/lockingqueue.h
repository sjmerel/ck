#pragma once
#include "ck/core/platform.h"
#include "ck/core/pool.h"
#include "ck/core/mutex.h"

namespace Cki
{


template <typename T>
class LockingQueue
{
public:
    LockingQueue(int size);
    ~LockingQueue();

    bool produce(const T& t);

    bool consume(T& result);

    void trim() {} 

private:
    struct Node 
    {
        Node(T val) : value(val), prev(NULL), next(NULL) {}
        T value;
        Node* prev;
        Node* next;
    };

    Node* m_first;
    Node* m_last;
    Mutex m_mutex;

    byte* m_buf;
    Pool m_pool;

};


}

