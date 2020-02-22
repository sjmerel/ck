#pragma once

#include "ck/core/platform.h"
#include <jni.h>

namespace Cki
{


class JniEnv
{
public:
    JniEnv();
    ~JniEnv();

    operator JNIEnv*();
    JNIEnv* operator->();

private:
    JNIEnv* m_jni;
};


////////////////////////////////////////

inline 
JniEnv::operator JNIEnv*()
{
    return m_jni;
}

inline 
JNIEnv* JniEnv::operator->()
{
    return m_jni;
}


}
