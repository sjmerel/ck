#pragma once

#include "ck/core/platform.h"
#include "ck/core/system.h"
#include "ck/core/fixedstring.h"
#include "ck/core/path.h"
#include <jni.h>

namespace Cki
{


class SystemAndroid : public System
{
public:
    static void init(const CkConfig&);
    static void shutdown();

    static SystemAndroid* get() { return s_instance; }

    int getSdkVersion() const; 

    JavaVM* getJavaVm() const;
    JNIEnv* getJniEnv() const; // for calling thread

    const char* getFilesDir() const; // e.g. /data/data/com.cricket.test/files
    const char* getExternalStorageDir() const ; // e.g. /mnt/sdcard

private:
    SystemAndroid(const CkConfig&);
    virtual ~SystemAndroid();

    int m_sdkVer;
    JavaVM* m_jvm;
    bool m_attached;
    FixedString<Path::k_maxLen> m_filesDir;
    FixedString<Path::k_maxLen> m_externalStorageDir;

    static SystemAndroid* s_instance;
    static char s_mem[];
};


}
