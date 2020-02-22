#pragma once

#include "ck/core/platform.h"
#undef verify // Apple assert macro

namespace Cki
{


template <typename T>
class TreeNode
{
public:
    TreeNode();
    ~TreeNode();

    T* getParent();
    T* getFirstChild();
    T* getPrevSibling();
    T* getNextSibling();

    T* getNext(); // iterate entire tree

    // add subtree as first child
    void addChild(T*);

    // remove this subtree from parent
    void remove(); 

#if CK_DEBUG
    void verify();
#endif

private:
    TreeNode<T>* m_parent;
    TreeNode<T>* m_firstChild;
    TreeNode<T>* m_prevSibling;
    TreeNode<T>* m_nextSibling;
};


}
