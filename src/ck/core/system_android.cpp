#include "ck/core/system_android.h"
#include "ck/core/debug.h"
#include "ck/core/logger.h"
#include <new>
#include <cpu-features.h>


namespace Cki
{

namespace
{
    void getString(JNIEnv* jni, jstring str, String& s)
    {
        // copy characters from java string
        const char* pathStr = jni->GetStringUTFChars(str, NULL);
        s = pathStr;
        jni->ReleaseStringUTFChars(str, pathStr);
    }

    void getStringField(JNIEnv* jni, jclass clazz, const char* name, String& s)
    {
        jfieldID fieldId = jni->GetStaticFieldID(clazz, name, "Ljava/lang/String;");
        jstring js = (jstring) jni->GetStaticObjectField(clazz, fieldId);
        if (js)
        {
            getString(jni, js, s);
        }
    }

    void getPathFromFile(JNIEnv* jni, jobject file, String& s)
    {
        // String path = file.getPath();
        jclass fileClass = jni->GetObjectClass(file);
        jmethodID getPathId = jni->GetMethodID(fileClass, "getPath", "()Ljava/lang/String;");
        jstring path = (jstring) jni->CallObjectMethod(file, getPathId);

        getString(jni, path, s);
    }
}

////////////////////////////////////////

void SystemAndroid::init(const CkConfig& config)
{
    if (!config.jni && !config.vm)
    {
        CK_LOG_ERROR("No JNIEnv or JavaVM pointer in config; cannot initialize");
        return;
    }
    if (!config.context)
    {
        CK_LOG_ERROR("No context pointer in config; cannot initialize");
        return;
    }

    if (!s_instance)
    {
        s_instance = new (s_mem) SystemAndroid(config);
    }
}

void SystemAndroid::shutdown()
{
    if (s_instance)
    {
        s_instance->~SystemAndroid();
        s_instance = NULL;
    }
}

int SystemAndroid::getSdkVersion() const
{
    return m_sdkVer;
}

JavaVM* SystemAndroid::getJavaVm() const
{
    return m_jvm;
}

JNIEnv* SystemAndroid::getJniEnv() const
{
    JNIEnv* jni;
    CK_VERIFY( JNI_OK == m_jvm->GetEnv((void**) &jni, JNI_VERSION_1_6) );
    return jni;
}

const char* SystemAndroid::getFilesDir() const
{
    return m_filesDir.getBuffer();
}

const char* SystemAndroid::getExternalStorageDir() const
{
    return m_externalStorageDir.getBuffer();
}

////////////////////////////////////////

SystemAndroid::SystemAndroid(const CkConfig& config) :
    System(config),
    m_sdkVer(0),
    m_jvm(NULL),
    m_attached(false),
    m_filesDir(),
    m_externalStorageDir()
{
    if (config.vm)
    {
        m_jvm = config.vm;
    }
    else
    {
        // get JVM
        CK_VERIFY(0 == config.jni->GetJavaVM(&m_jvm));
    }

    JNIEnv* jni;
    if (m_jvm->GetEnv((void**) &jni, JNI_VERSION_1_6) == JNI_EDETACHED)
    {
        // need to attach to thread
        JavaVMAttachArgs args = { 0 };
        args.version = JNI_VERSION_1_6;
        m_jvm->AttachCurrentThread(&jni, &args);
        m_attached = true;
    }

    jobject context = config.context;

    jni->ExceptionClear();

    jclass contextClass = jni->GetObjectClass(context);

    {
        jclass versionClass = jni->FindClass("android/os/Build$VERSION");
        jfieldID fieldId = jni->GetStaticFieldID(versionClass, "SDK_INT", "I");
        m_sdkVer = jni->GetStaticIntField(versionClass, fieldId);
    }

    {
        // dir = context.getFilesDir();
        jmethodID methodId = jni->GetMethodID(contextClass, "getFilesDir", "()Ljava/io/File;");
        jobject dir = jni->CallObjectMethod(context, methodId);

        getPathFromFile(jni, dir, m_filesDir);
//        CK_PRINT("files dir: %s\n", m_filesDir.getBuffer());
    }

    {
        // dir = Environment.getExternalStorageDirectory();
        jclass environmentClass = jni->FindClass("android/os/Environment");
        jmethodID methodId = jni->GetStaticMethodID(environmentClass, "getExternalStorageDirectory", "()Ljava/io/File;");
        jobject dir = jni->CallStaticObjectMethod(environmentClass, methodId);

        getPathFromFile(jni, dir, m_externalStorageDir);
//        CK_PRINT("external storage dir: %s\n", m_externalStorageDir.getBuffer());
    }

#if __ARM_ARCH_7A__ 
    CK_LOG_INFO("built for armeabi-v7a");
#endif
#if __ARM_ARCH_5TE__ 
    CK_LOG_INFO("built for armeabi");
#endif
#if __i686__
    CK_LOG_INFO("built for x86");
#endif
#if __mips__
    CK_LOG_INFO("built for mips");
#endif
    const char* cpuFamilyName = NULL;
#define HANDLE_CPU_FAMILY(x) case ANDROID_CPU_FAMILY_ ## x: cpuFamilyName = #x; break
    AndroidCpuFamily cpuFamily = android_getCpuFamily();
    switch (cpuFamily)
    {
        HANDLE_CPU_FAMILY(ARM);
        HANDLE_CPU_FAMILY(X86);
        HANDLE_CPU_FAMILY(MIPS);
        HANDLE_CPU_FAMILY(ARM64);
        HANDLE_CPU_FAMILY(X86_64);
        HANDLE_CPU_FAMILY(MIPS64);
        default: cpuFamilyName = "unknown";
    }
    CK_LOG_INFO("processor family: %s\n", cpuFamilyName);

    uint64 features = android_getCpuFeatures();
    if (cpuFamily == ANDROID_CPU_FAMILY_ARM)
    {
        bool vfp = features & ANDROID_CPU_ARM_FEATURE_VFPv3;
        bool neon = features & ANDROID_CPU_ARM_FEATURE_NEON;
        bool armv7 = features & ANDROID_CPU_ARM_FEATURE_ARMv7;
        CK_LOG_INFO("processor features: armv7=%d, vfp=%d, neon=%d", armv7, vfp, neon);
        m_simd = neon;
    }
    else if (cpuFamily == ANDROID_CPU_FAMILY_ARM64)
    {
        bool fp = features & ANDROID_CPU_ARM64_FEATURE_FP;
        bool neon = features & ANDROID_CPU_ARM64_FEATURE_ASIMD;
        CK_LOG_INFO("processor features: fp=%d, asimd=%d", fp, neon);
        m_simd = neon;
    }
    else if (cpuFamily == ANDROID_CPU_FAMILY_X86)
    {
        m_simd = features & ANDROID_CPU_X86_FEATURE_SSSE3;
    }
    // TODO ANDROID_CPU_FAMILY_X86_64 ?


    CK_LOG_INFO("processor cores: %d\n", android_getCpuCount());

    {
        jclass buildClass = jni->FindClass("android/os/Build");

        FixedString<64> model;
        getStringField(jni, buildClass, "MODEL", model);
        FixedString<64> device;
        getStringField(jni, buildClass, "DEVICE", device);
        FixedString<64> product;
        getStringField(jni, buildClass, "PRODUCT", product);
        FixedString<64> manufacturer;
        getStringField(jni, buildClass, "MANUFACTURER", manufacturer);
        FixedString<64> brand;
        getStringField(jni, buildClass, "BRAND", brand);

        CK_LOG_INFO("device: sdk=%d, model=%s, device=%s, product=%s, manufacturer=%s, brand=%s\n", 
                m_sdkVer,
                model.getBuffer(),
                device.getBuffer(),
                product.getBuffer(),
                manufacturer.getBuffer(),
                brand.getBuffer());
    }

    CK_ASSERT(!jni->ExceptionOccurred());
}

SystemAndroid::~SystemAndroid()
{
    if (m_attached)
    {
        m_jvm->DetachCurrentThread();
    }
}


SystemAndroid* SystemAndroid::s_instance = NULL;
char SystemAndroid::s_mem[sizeof(SystemAndroid)];



}
