#include "ck/core/jnienv_android.h"
#include "ck/core/debug.h"
#include "ck/core/system_android.h"

namespace Cki
{


JniEnv::JniEnv() : 
    m_jni(SystemAndroid::get()->getJniEnv()) 
{
#if CK_ASSERTS_ENABLED
    if (m_jni->ExceptionCheck())
    {
        m_jni->ExceptionDescribe();
        CK_FAIL("jni exception in ctor");
    }
#endif
}

JniEnv::~JniEnv()
{
#if CK_ASSERTS_ENABLED
    if (m_jni->ExceptionCheck())
    {
        m_jni->ExceptionDescribe();
        CK_FAIL("jni exception");
    }
#endif
}


}
