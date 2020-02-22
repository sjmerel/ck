#include "ck/core/deletable.h"
#include "ck/core/mutexlock.h"
#include "ck/core/debug.h"
#include "ck/core/thread.h"

// template instantiations
#include "ck/core/list.cpp"

namespace Cki
{

void Deletable::deleteOnUpdate()
{
    CK_ASSERT(s_inited);
    MutexLock lock(s_mutex);
    s_deleteList.addFirst(this);
}

void Deletable::init()
{
    s_inited = true;
}

void Deletable::update()
{
    CK_ASSERT(s_inited);
    MutexLock lock(s_mutex);
    Deletable* p = s_deleteList.getFirst();
    while (p)
    {
        Deletable* next = p->getNext();
        if (p->isReadyToDelete())
        {
            s_deleteList.remove(p);
            delete p;
        }
        p = next;
    }
}

void Deletable::shutdown()
{
    for (;;)
    {
        update();

        // even after update(), list may not be empty because some objects
        // were not ready to delete, so may need to wait and try again
        bool empty;
        {
            MutexLock lock(s_mutex);
            empty = s_deleteList.isEmpty();
        }

        if (empty)
        {
            break;
        }
        else
        {
            Thread::sleepMs(15);
        }
    }

    s_inited = false;
}

List<Deletable> Deletable::s_deleteList;
Mutex Deletable::s_mutex;
bool Deletable::s_inited = false;

template class List<Deletable>;

}
