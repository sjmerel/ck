#pragma once

#include "ck/core/platform.h"
#include "ck/core/types.h"
#include <jni.h>

namespace Cki
{


class JavaStringRef
{
public:
    JavaStringRef(JNIEnv* jni, jstring s) :
        m_jni(jni),
        m_string(s)
    {
        if (m_string)
        {
            m_chars = jni->GetStringUTFChars(s, 0);
        }
        else
        {
            m_chars = NULL;
        }
    }

    ~JavaStringRef()
    {
        if (m_string)
        {
            m_jni->ReleaseStringUTFChars(m_string, m_chars);
        }
    }

    const char* getChars()
    {
        return m_chars;
    }

private:
    JNIEnv* m_jni;
    jstring m_string;
    const char* m_chars;
};


}

