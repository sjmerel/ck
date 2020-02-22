#include "floatref.h"

namespace
{
    jmethodID g_getId = 0;
    jmethodID g_setId = 0;
}

namespace Cki
{

namespace FloatRef
{

float get(JNIEnv* jni, jobject obj)
{
    return jni->CallFloatMethod(obj, g_getId);
}

void set(JNIEnv* jni, jobject obj, float value)
{
    if (obj)
    {
        jni->CallVoidMethod(obj, g_setId, value);
    }
}

void init(JNIEnv* jni)
{
    jclass clazz = jni->FindClass("com/crickettechnology/audio/FloatRef");
    g_getId = jni->GetMethodID(clazz, "get", "()F");
    g_setId = jni->GetMethodID(clazz, "set", "(F)V");
}

}

}
