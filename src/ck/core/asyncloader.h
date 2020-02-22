#pragma once

#include "ck/core/platform.h"
#include "ck/core/allocatable.h"
#include "ck/core/list.h"
#include "ck/core/thread.h"
#include "ck/core/cond.h"
#include "ck/core/mutex.h"
#include "ck/core/fixedstring.h"
#include "ck/core/path.h"
#include "ck/core/callback.h"
#include "ck/core/staticsingleton.h"
#include "ck/pathtype.h"

namespace Cki
{

class BinaryStream;

class AsyncLoader : public StaticSingleton<AsyncLoader>
{
public:
    bool load(const char* path, CkPathType, void* buf, int offset, int size, Callback1<bool>::Func, void* data);

private:
    struct Request : public Allocatable, public List<Request>::Node
    {
        FixedString<Path::k_maxLen> m_path;
        CkPathType m_pathType;
        Callback1<bool> m_callback;
        void* m_buf;
        int m_offset;
        int m_size;
    };

    List<Request> m_requests;
    Thread m_thread;
    bool m_stop;
    Mutex m_mutex;
    Cond m_cond;

    static void* threadFunc(void*);
    void threadLoop();
    void read(BinaryStream&, Request*);

    friend class StaticSingleton<AsyncLoader>;
    AsyncLoader();
    ~AsyncLoader();
};

// silence warning
extern template class StaticSingleton<AsyncLoader>;

}
