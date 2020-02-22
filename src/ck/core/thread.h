#pragma once

#include "ck/core/platform.h"
#include "ck/core/types.h"
#if CK_PLATFORM_WIN 
#  include <windows.h>
#elif CK_PLATFORM_WP8
#  include <thread>
#else
#  include <pthread.h>
#endif

namespace Cki
{


class Thread
{
public:
    typedef void* (*ThreadFunc)(void* arg);


#if CK_PLATFORM_ANDROID
    // Android threads that will call JVM functions must attach to the JVM.
    enum { k_flagAttachToJvm = 1 };
#endif

    Thread(ThreadFunc);
    ~Thread();

    // TODO:
    // - thread stack & size

    void start(void* arg = NULL);
    bool isRunning() const;

#if !CK_PLATFORM_WP8
    void setPriority(int);
    int getPriority() const;
#endif

    void setFlags(uint32 flags);
    uint32 getFlags() const;

    void setName(const char*);
    const char* getName() const;

    int getId() const;
    void join();
    void* getReturnValue() const;

    // priority values
    // NOTE: these vary with platform!
    // Also, on Android, the min/max priorities are negated.
#if !CK_PLATFORM_WP8
    static const int k_priorityDefault;
    static const int k_priorityMin;
    static const int k_priorityMax;
#endif

    static int getCurrentThreadId();
    static void sleepMs(int ms);

private:
    ThreadFunc m_func;
#if !CK_PLATFORM_WP8
    int m_priority;
#endif
    const char* m_name;
    void* m_arg;
    void* m_result;
    bool m_running;
    int m_id;
    uint32 m_flags;
#if CK_PLATFORM_WIN 
    HANDLE m_thread;
#elif CK_PLATFORM_WP8
    std::thread m_thread;
#else
    pthread_t m_thread;
#endif

#if CK_PLATFORM_WIN 
    static DWORD WINAPI runFunc(void*);
#else
    static void* runFunc(void*);
#endif

    void run();

    Thread(const Thread&);
    Thread& operator=(const Thread&);

};


}
