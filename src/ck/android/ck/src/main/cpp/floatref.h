#pragma once

#include "ck/core/platform.h"
#include <jni.h>

namespace Cki
{


namespace FloatRef
{
    float get(JNIEnv* jni, jobject obj);
    void set(JNIEnv* jni, jobject obj, float);

    void init(JNIEnv* jni);
};


}
