#include "ck/core/asyncloader.h"
#include "ck/core/debug.h"
#include "ck/core/readstream.h"

// template instantiation
#include "ck/core/list.cpp" 
#include "ck/core/staticsingleton.cpp" 

namespace Cki
{


bool AsyncLoader::load(const char* path, CkPathType pathType, void* buf, int offset, int size, Callback1<bool>::Func callback, void* data)
{
    m_mutex.lock();

    CK_ASSERT(buf);
    Request* req = new Request();
    if (req)
    {
        req->m_path = path;
        req->m_pathType = pathType;
        req->m_callback.set(callback, data);
        req->m_buf = buf;
        req->m_offset = offset;
        req->m_size = size;

        m_requests.addLast(req);

        m_cond.signal();
        m_mutex.unlock();

        return true;
    }
    else
    {
        m_mutex.unlock();
        return false;
    }
}

////////////////////////////////////////

AsyncLoader::AsyncLoader() :
    m_requests(),
    m_thread(threadFunc),
    m_stop(false),
    m_cond()
{
#if CK_PLATFORM_ANDROID
    m_thread.setFlags(Thread::k_flagAttachToJvm);
#endif
    m_thread.setName("CK async loader");
    m_thread.start(this);
}

AsyncLoader::~AsyncLoader()
{
    // signal, in case thread is waiting
    m_mutex.lock();
    m_stop = true;
    m_cond.signal();
    m_mutex.unlock();

    m_thread.join();

    Request* req = NULL;
    while ((req = m_requests.getFirst()))
    {
        m_requests.remove(req);
        delete req;
    }
}

void* AsyncLoader::threadFunc(void* arg)
{
    AsyncLoader* mgr = (AsyncLoader*) arg;
    mgr->threadLoop();
    return NULL;
}

void AsyncLoader::threadLoop()
{
    while (!m_stop)
    {
        // find a request
        m_mutex.lock();
        Request* req = m_requests.getFirst();
        if (req)
        {
            m_requests.remove(req);
        }
        m_mutex.unlock();

        if (req)
        {
            // process request
            ReadStream stream(req->m_path.getBuffer(), req->m_pathType);
            read(stream, req);
            delete req;
        }
        else
        {
            m_mutex.lock();
            if (!m_stop)
            {
                m_cond.wait(m_mutex);
            }
            m_mutex.unlock();
        }
    }
}

void AsyncLoader::read(BinaryStream& stream, Request* req)
{
    if (stream.isValid())
    {
        stream.setPos(req->m_offset);
        int bytesRead = stream.read(req->m_buf, req->m_size);
        req->m_callback.call(bytesRead == req->m_size);
    }
    else
    {
        req->m_callback.call(false);
    }
}

template class StaticSingleton<AsyncLoader>;

}
