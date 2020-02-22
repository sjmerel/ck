#pragma once

#include "ck/core/platform.h"
#include "ck/core/mutex.h"

namespace Cki
{


template <typename T>
class TaskQueue
{
public:
    TaskQueue(int capacity);
    ~TaskQueue();

    bool produce(const T&);
    bool consume(T& result);
    bool isEmpty() const;
    int getCapacity() const;

private:
    int m_capacity;
    T* m_tasks;
    int m_read;
    int m_write;
    Mutex m_mutex;

    int next(int);

};


}
