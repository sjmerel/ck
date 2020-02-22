#include "ck/core/thread.h"
#include "ck/core/debug.h"

#if CK_PLATFORM_ANDROID || CK_PLATFORM_OSX || CK_PLATFORM_IOS || CK_PLATFORM_LINUX || CK_PLATFORM_TVOS
#  include <unistd.h>
#endif

#if CK_PLATFORM_LINUX
#  include <sys/syscall.h>
#endif

#if CK_PLATFORM_ANDROID 
#  include "ck/core/system_android.h"
#  include <jni.h>
#  include <sys/resource.h>
#endif

#if CK_PLATFORM_WP8
#  include <sstream>
#endif

namespace
{
#if CK_PLATFORM_WIN 
    // set thread name
    // see http://msdn.microsoft.com/en-us/library/windows/desktop/ee416321(v=vs.85).aspx

    struct THREADNAME_INFO
    {
        DWORD dwType;     // must be 0x1000
        LPCSTR szName;    // pointer to name (in user address space)
        DWORD dwThreadID; // thread ID (-1 = caller thread)
        DWORD dwFlags;    // reserved for future use, must be zero
    };

    void setThreadName(DWORD dwThreadID, LPCSTR szThreadName)
    {
        THREADNAME_INFO info;
        info.dwType = 0x1000;
        info.szName = szThreadName;
        info.dwThreadID = dwThreadID;
        info.dwFlags = 0;

        __try
        {
            RaiseException(0x406D1388, 0,
                    sizeof(info) / sizeof(DWORD),
                    (const ULONG_PTR*)&info);
        }
        __except(EXCEPTION_CONTINUE_EXECUTION) {}
    }
#endif
}

namespace Cki
{

Thread::Thread(ThreadFunc func) :
    m_func(func),
#if !CK_PLATFORM_WP8
    m_priority(k_priorityDefault),
#endif
    m_name(NULL),
    m_arg(NULL),
    m_result(NULL),
    m_running(false),
    m_id(0),
    m_flags(0)
{
#if CK_PLATFORM_WIN 
    m_thread = NULL;
#endif
}

Thread::~Thread()
{
    join();
#if CK_PLATFORM_WIN 
    if (m_thread)
    {
        CK_VERIFY( CloseHandle(m_thread) );
    }
#endif
}

void Thread::start(void* data)
{
    CK_ASSERT(!m_running);
    m_running = true;
    m_arg = data;

#if CK_PLATFORM_WIN 
    if (m_thread)
    {
        CK_VERIFY( CloseHandle(m_thread) );
    }
    CK_VERIFY( m_thread = CreateThread(NULL, 0, runFunc, this, CREATE_SUSPENDED, NULL) );
    CK_VERIFY( SetThreadPriority(m_thread, m_priority) );
    CK_VERIFY( ResumeThread(m_thread) != -1 );
#elif CK_PLATFORM_WP8
    m_thread = std::thread(runFunc, this);
    // TODO set priority
#else
    pthread_attr_t attr;
    CK_VERIFY( 0 == pthread_attr_init(&attr) );
    CK_VERIFY( 0 == pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE) );

#  if CK_PLATFORM_IOS || CK_PLATFORM_OSX || CK_PLATFORM_TVOS
    sched_param param;
    CK_VERIFY( 0 == pthread_attr_getschedparam(&attr, &param) );
#    if CK_DEBUG
    int policy;
    CK_VERIFY( 0 == pthread_attr_getschedpolicy(&attr, &policy) );
    CK_ASSERT(sched_get_priority_min(policy) == k_priorityMin);
    CK_ASSERT(sched_get_priority_max(policy) == k_priorityMax);
    CK_ASSERT(param.sched_priority == k_priorityDefault);
#    endif
    param.sched_priority = m_priority;
    CK_VERIFY( 0 == pthread_attr_setschedparam(&attr, &param) );
#  endif

    CK_VERIFY( 0 == pthread_create(&m_thread, NULL, runFunc, this) );
    CK_VERIFY( 0 == pthread_attr_destroy(&attr) );
#endif
}

bool Thread::isRunning() const
{
    return m_running;
}

#if !CK_PLATFORM_WP8
void Thread::setPriority(int priority)
{
    CK_ASSERT(priority >= k_priorityMin && priority <= k_priorityMax);
    CK_ASSERT(!m_running);
    m_priority = priority;
}

int Thread::getPriority() const
{
    return m_priority;
}
#endif

void Thread::setFlags(uint32 flags)
{
    CK_ASSERT(!m_running);
    m_flags = flags;
}

uint32 Thread::getFlags() const
{
    return m_flags;
}

void Thread::setName(const char* name)
{
    CK_ASSERT(!m_running);
    m_name = name;
}

const char* Thread::getName() const
{
    return m_name;
}

int Thread::getId() const
{
    return m_id;
}

void Thread::join()
{
#if CK_PLATFORM_WP8
    if (m_thread.joinable())
    {
        m_thread.join();
    }
#else
    if (m_running)
    {
#if CK_PLATFORM_WIN 
        CK_VERIFY( WaitForSingleObject(m_thread, INFINITE) == WAIT_OBJECT_0 );
#else
        CK_VERIFY( 0 == pthread_join(m_thread, NULL) );
#endif
    }
#endif
}

void* Thread::getReturnValue() const
{
    return m_result;
}

int Thread::getCurrentThreadId()
{
#if CK_PLATFORM_WIN 
    return GetCurrentThreadId();
#elif CK_PLATFORM_WP8
    std::ostringstream s;
    s << std::this_thread::get_id();
    return std::stoi(s.str());
#elif CK_PLATFORM_ANDROID
    return gettid();
#elif CK_PLATFORM_LINUX
    return syscall(__NR_gettid);
#elif CK_PLATFORM_IOS || CK_PLATFORM_OSX || CK_PLATFORM_TVOS
    return pthread_mach_thread_np(pthread_self());
#endif
}

void Thread::sleepMs(int ms)
{
#if CK_PLATFORM_WIN
    ::Sleep(ms);
#elif CK_PLATFORM_WP8
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
#else
    usleep(ms * 1000);
#endif
}

////////////////////////////////////////

#if CK_PLATFORM_WIN
DWORD Thread::runFunc(void* arg)
#else
void* Thread::runFunc(void* arg)
#endif
{
    Thread* thread = (Thread*) arg;
    thread->run();

    return 0;
}

void Thread::run()
{
#if CK_PLATFORM_ANDROID
    if (m_flags & k_flagAttachToJvm)
    {
        // attach JVM to current thread
        SystemAndroid* system = SystemAndroid::get();
        JavaVM* jvm = system->getJavaVm();
        JavaVMAttachArgs args = { 0 };
        args.version = JNI_VERSION_1_6;
        JNIEnv* jnip;
        CK_VERIFY( JNI_OK == jvm->AttachCurrentThread(&jnip, &args) );
    }

    // set priority
    // note that priority values are negated, so lower values correspond to lower priority
    pid_t id = gettid();
    CK_VERIFY( 0 == setpriority(PRIO_PROCESS, id, -m_priority));
    CK_ASSERT(getpriority(PRIO_PROCESS, id) == -m_priority);
#endif

    m_id = getCurrentThreadId();

    if (m_name)
    {
#if CK_PLATFORM_WIN 
        setThreadName(m_id, m_name);
#elif CK_PLATFORM_ANDROID
//        CK_VERIFY( 0 == pthread_setname_np(m_thread, m_name) ); // defined in pthread.h but doesn't link
#elif CK_PLATFORM_IOS || CK_PLATFORM_OSX || CK_PLATFORM_TVOS
        CK_VERIFY( 0 == pthread_setname_np(m_name) ); // name doesn't actually seem to show up in iOS, alas
#endif
    }

    m_result = m_func(m_arg);

#if CK_PLATFORM_ANDROID
    if (m_flags & k_flagAttachToJvm)
    {
        // detach from JVM
        JavaVM* jvm = SystemAndroid::get()->getJavaVm();
        CK_VERIFY( JNI_OK == jvm->DetachCurrentThread() );
    }
#endif

    m_running = false;
    m_id = 0;
#if CK_PLATFORM_ANDROID || CK_PLATFORM_IOS || CK_PLATFORM_OSX || CK_PLATFORM_TVOS
    pthread_exit(0);
#endif
}

#if CK_PLATFORM_WIN 
const int Thread::k_priorityDefault = 0;
const int Thread::k_priorityMin = THREAD_PRIORITY_LOWEST;
const int Thread::k_priorityMax =THREAD_PRIORITY_HIGHEST;
#elif CK_PLATFORM_OSX || CK_PLATFORM_IOS || CK_PLATFORM_LINUX || CK_PLATFORM_TVOS
const int Thread::k_priorityDefault = 31;
const int Thread::k_priorityMin = 15;
const int Thread::k_priorityMax = 47;
#elif CK_PLATFORM_ANDROID
const int Thread::k_priorityDefault = 0;
const int Thread::k_priorityMin = -19;
const int Thread::k_priorityMax = 19;
#endif

}
