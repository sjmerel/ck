#pragma once

#include "ck/core/platform.h"
#include <jni.h>

namespace Cki
{


namespace IntRef
{
    int get(JNIEnv* jni, jobject obj);
    void set(JNIEnv* jni, jobject obj, int);

    void init(JNIEnv* jni);
};


}
