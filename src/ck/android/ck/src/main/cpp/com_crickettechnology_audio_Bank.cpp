#include <jni.h>
#include "ck/bank.h"
#include "ck/core/javastringref_android.h"

extern "C"
{


jboolean Java_com_crickettechnology_audio_Bank_nativeIsLoaded(JNIEnv* env, jclass, jlong inst)
{
    CkBank* bank = (CkBank*) inst;
    return bank->isLoaded();
}

jboolean Java_com_crickettechnology_audio_Bank_nativeIsFailed(JNIEnv* env, jclass, jlong inst)
{
    CkBank* bank = (CkBank*) inst;
    return bank->isFailed();
}

jstring Java_com_crickettechnology_audio_Bank_nativeGetName(JNIEnv* env, jclass, jlong inst)
{
    CkBank* bank = (CkBank*) inst;
    const char* name = bank->getName();
    return env->NewStringUTF(name);
}

jint Java_com_crickettechnology_audio_Bank_nativeGetNumSounds(JNIEnv*, jclass, jlong inst)
{
    CkBank* bank = (CkBank*) inst;
    return bank->getNumSounds();
}

jstring Java_com_crickettechnology_audio_Bank_nativeGetSoundName(JNIEnv* env, jclass, jlong inst, jint index)
{
    CkBank* bank = (CkBank*) inst;
    const char* name = bank->getSoundName(index);
    return env->NewStringUTF(name);
}

jlong Java_com_crickettechnology_audio_Bank_nativeNewBank(JNIEnv* env, jclass, jstring pathStr, jint pathType, jint offset, jint length)
{
    Cki::JavaStringRef path(env, pathStr);
    CkBank* bank = CkBank::newBank(path.getChars(), (CkPathType) pathType, offset, length);
    return (jlong) bank;
}

jlong Java_com_crickettechnology_audio_Bank_nativeNewBankAsync(JNIEnv* env, jclass, jstring pathStr, jint pathType, jint offset, jint length)
{
    Cki::JavaStringRef path(env, pathStr);
    CkBank* bank = CkBank::newBankAsync(path.getChars(), (CkPathType) pathType, offset, length);
    return (jlong) bank;
}

jlong Java_com_crickettechnology_audio_Bank_nativeFind(JNIEnv* env, jclass, jstring nameStr)
{
    Cki::JavaStringRef name(env, nameStr);
    CkBank* bank = CkBank::find(name.getChars());
    return (jlong) bank;
}

void Java_com_crickettechnology_audio_Bank_nativeDestroy(JNIEnv*, jclass, jlong inst)
{
    CkBank* bank = (CkBank*) inst;
    bank->destroy();
}


}
