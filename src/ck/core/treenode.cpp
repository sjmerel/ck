#include "ck/core/treenode.h"
#include "ck/core/debug.h"

namespace Cki
{

template <typename T>
TreeNode<T>::TreeNode() : 
    m_parent(NULL), 
    m_firstChild(NULL), 
    m_prevSibling(NULL),
    m_nextSibling(NULL) 
{
}

template <typename T>
TreeNode<T>::~TreeNode()
{
    remove();
    TreeNode<T>* child = m_firstChild;
    while (child)
    {
        child->remove();
        child = child->m_nextSibling;
    }
}

template <typename T>
T* TreeNode<T>::getParent() 
{ 
    return (T*) m_parent; 
}

template <typename T>
T* TreeNode<T>::getFirstChild() 
{ 
    return (T*) m_firstChild; 
}

template <typename T>
T* TreeNode<T>::getPrevSibling() 
{ 
    return (T*) m_prevSibling; 
}

template <typename T>
T* TreeNode<T>::getNextSibling() 
{ 
    return (T*) m_nextSibling; 
}

template <typename T>
T* TreeNode<T>::getNext()
{
    if (m_firstChild)
    {
        return (T*) m_firstChild;
    }
    else if (m_nextSibling)
    {
        return (T*) m_nextSibling;
    }
    else 
    {
        TreeNode<T>* p = this;
        while (p)
        {
            p = p->m_parent;
            if (p && p->m_nextSibling)
            {
                return (T*) p->m_nextSibling;
            }
        }
        return (T*) p;
    }
}

template <typename T>
void TreeNode<T>::addChild(T* t)
{
    TreeNode<T>* node = (TreeNode<T>*) t;
    CK_ASSERT(node != this);

    if (node->m_parent == this)
    {
        return; // already a child
    }

    node->remove();
    node->m_parent = this;
    node->m_nextSibling = m_firstChild;
    if (m_firstChild)
    {
        m_firstChild->m_prevSibling = node;
    }
    m_firstChild = node;
}

template <typename T>
void TreeNode<T>::remove()
{
    if (m_parent)
    {
        if (m_prevSibling)
        {
            m_prevSibling->m_nextSibling = m_nextSibling;
        }
        else
        {
            m_parent->m_firstChild = m_nextSibling;
        }

        if (m_nextSibling)
        {
            m_nextSibling->m_prevSibling = m_prevSibling;
        }

        m_prevSibling = NULL;
        m_nextSibling = NULL;
        m_parent = NULL;
    }
}

#if CK_DEBUG
template <typename T>
void TreeNode<T>::verify()
{
    TreeNode<T>* child = m_firstChild;
    CK_ASSERT(!child || !child->m_prevSibling);
    while (child)
    {
        CK_ASSERT(child->m_parent == this);
        CK_ASSERT(child->m_nextSibling == NULL || child->m_nextSibling->m_prevSibling == child);
        CK_ASSERT(child->m_prevSibling == NULL || child->m_prevSibling->m_nextSibling == child);
        child->verify();
        child = child->m_nextSibling;
    }
}
#endif

}
