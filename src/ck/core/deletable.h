#pragma once

#include "ck/core/platform.h"
#include "ck/core/list.h"
#include "ck/core/mutex.h"

namespace Cki
{


// Base class for delayed deletion (i.e. for classes that need to be deleted on the main thread)

class Deletable : public List<Deletable>::Node
{
public:
    virtual ~Deletable() {}

    virtual bool isReadyToDelete() const { return true; }

    void deleteOnUpdate();

    static void init();
    static void update();
    static void shutdown();

private:
    static List<Deletable> s_deleteList;
    static Mutex s_mutex;
    static bool s_inited;
};


}

