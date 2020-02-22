#include "intref.h"

namespace
{
    jmethodID g_getId = 0;
    jmethodID g_setId = 0;
}

namespace Cki
{

namespace IntRef
{

int get(JNIEnv* jni, jobject obj)
{
    return jni->CallIntMethod(obj, g_getId);
}

void set(JNIEnv* jni, jobject obj, int value)
{
    if (obj)
    {
        jni->CallVoidMethod(obj, g_setId, value);
    }
}

void init(JNIEnv* jni)
{
    jclass clazz = jni->FindClass("com/crickettechnology/audio/IntRef");
    g_getId = jni->GetMethodID(clazz, "get", "()I");
    g_setId = jni->GetMethodID(clazz, "set", "(I)V");
}

}

}
