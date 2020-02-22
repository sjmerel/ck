#pragma once

#include "ck/core/platform.h"
#include "ck/core/types.h"
#undef verify // Apple assert macro

namespace Cki
{


template <typename T, int N = 0>
class List
{
public:

    ////////////////////////////////////////

    class Node
    {
    public:
        Node() : m_prev(NULL), m_next(NULL) {}

        T* getPrev() { return (T*) m_prev; }
        T* getNext() { return (T*) m_next; }

    private:
        Node* m_prev;
        Node* m_next;
        friend class List;
    };

    ////////////////////////////////////////

    List();
    ~List();

    T* getFirst();
    T* getLast();

    void addFirst(T*);
    void addLast(T*);

    void addBefore(T*, T* pos);
    void addAfter(T*, T* pos);
    bool contains(T*) const;
    int getSize() const;
    bool isEmpty() const;
    void remove(T*);
    void clear();

#if CK_DEBUG
    void verify();
#endif

private:
    Node* m_first;
    Node* m_last;
    int m_size;
};



}
