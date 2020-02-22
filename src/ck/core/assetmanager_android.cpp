#include "ck/core/assetmanager_android.h"
#include "ck/core/system_android.h"
#include "ck/core/debug.h"
#include "ck/core/math.h"
#include "ck/core/allocatable.h"
#include "ck/core/jnienv_android.h"
#include "ck/core/mem.h"
#include <android/asset_manager.h>
#include <dlfcn.h>
#include <new>
#include <stdio.h>

//#define USE_JAVA_ASSETMGR 1

namespace Cki
{

namespace AssetManager
{
    enum { k_bufSize = 20480 };

    struct AAsset : public Allocatable
    {
        jobject m_stream;
        int m_size;
        int m_pos;
        jbyteArray m_buf;
    };

    CK_STATIC_ASSERT(k_assetSize == sizeof(AAsset));

    namespace
    {
        class AssetManagerBase
        {
        public:
            virtual ~AssetManagerBase() {}
            virtual AAsset* open(const char* filename, int mode, void* assetBuf) = 0;
            virtual int read(AAsset*, void* buf, size_t count) = 0;
            virtual off_t seek(AAsset*, off_t offset) = 0;
            virtual off_t getLength(AAsset*) = 0;
            virtual int openFileDescriptor(AAsset*, off_t* start, off_t* length) = 0;
            virtual void close(AAsset*) = 0;
        };

        ////////////////////////////////////////

        class AssetManagerNative : public AssetManagerBase
        {
        public:
            AssetManagerNative(jobject assetMgr) :
                m_handle(NULL),
                m_assetMgr(NULL),
                m_assetMgrRef(NULL),
                m_openFunc(NULL),
                m_readFunc(NULL),
                m_seekFunc(NULL),
                m_getLengthFunc(NULL),
                m_closeFunc(NULL)
            {
                m_handle = dlopen("libandroid.so", RTLD_NOW | RTLD_GLOBAL);
                CK_ASSERT(m_handle);

                m_openFunc = (AAssetManager_openFunc) dlsym(m_handle, "AAssetManager_open");
                m_readFunc = (AAsset_readFunc) dlsym(m_handle, "AAsset_read");
                m_seekFunc = (AAsset_seekFunc) dlsym(m_handle, "AAsset_seek");
                m_getLengthFunc = (AAsset_getLengthFunc) dlsym(m_handle, "AAsset_getLength");
                m_openFileDescriptorFunc = (AAsset_openFileDescriptorFunc) dlsym(m_handle, "AAsset_openFileDescriptor");
                m_closeFunc = (AAsset_closeFunc) dlsym(m_handle, "AAsset_close");

                typedef AAssetManager* (*AAssetManager_fromJavaFunc)(JNIEnv*, jobject);
                AAssetManager_fromJavaFunc fromJava = (AAssetManager_fromJavaFunc) dlsym(m_handle, "AAssetManager_fromJava");

                JniEnv jni;
                m_assetMgr = fromJava(jni, assetMgr);
                m_assetMgrRef = jni->NewGlobalRef(assetMgr);
            }

            virtual ~AssetManagerNative()
            {
                CK_VERIFY( dlclose(m_handle) == 0 );

                JniEnv jni;
                jni->DeleteGlobalRef(m_assetMgrRef);
            }

            virtual AAsset* open(const char* filename, int mode, void* assetBuf)
            {
                return m_openFunc(m_assetMgr, filename, mode);
            }

            virtual int read(AAsset* asset, void* buf, size_t count)
            {
                return m_readFunc(asset, buf, count);
            }

            virtual off_t seek(AAsset* asset, off_t offset)
            {
                return m_seekFunc(asset, offset, SEEK_SET);
            }

            virtual off_t getLength(AAsset* asset)
            {
                return m_getLengthFunc(asset);
            }

            virtual int openFileDescriptor(AAsset* asset, off_t* start, off_t* length)
            {
                return m_openFileDescriptorFunc(asset, start, length);
            }

            virtual void close(AAsset* asset)
            {
                return m_closeFunc(asset);
            }


        private:
            void* m_handle;
            AAssetManager* m_assetMgr;
            jobject m_assetMgrRef;

            typedef AAsset* (*AAssetManager_openFunc)(AAssetManager* mgr, const char* filename, int mode);
            typedef int (*AAsset_readFunc)(AAsset*, void* buf, size_t count);
            typedef off_t (*AAsset_seekFunc)(AAsset*, off_t offset, int whence);
            typedef off_t (*AAsset_getLengthFunc)(AAsset*);
            typedef int (*AAsset_openFileDescriptorFunc)(AAsset* asset, off_t* start, off_t* length);
            typedef void (*AAsset_closeFunc)(AAsset*);

            AAssetManager_openFunc m_openFunc;
            AAsset_readFunc m_readFunc;
            AAsset_seekFunc m_seekFunc;
            AAsset_getLengthFunc m_getLengthFunc;
            AAsset_openFileDescriptorFunc m_openFileDescriptorFunc;
            AAsset_closeFunc m_closeFunc;
        };

        ////////////////////////////////////////

        class AssetManagerJava : public AssetManagerBase
        {
        public:
            AssetManagerJava(jobject assetMgr) :
                m_assetMgr(NULL),
                m_openId(NULL),
                m_readId(NULL),
                m_availableId(NULL),
                m_markId(NULL),
                m_skipId(NULL),
                m_resetId(NULL)
            {
                JniEnv jni;
                m_assetMgr = jni->NewGlobalRef(assetMgr);

                jclass assetMgrClass = jni->GetObjectClass(assetMgr);
                m_openId = jni->GetMethodID(assetMgrClass, "open", "(Ljava/lang/String;I)Ljava/io/InputStream;");

                jclass inputStreamClass = jni->FindClass("java/io/InputStream");
                m_readId = jni->GetMethodID(inputStreamClass, "read", "([BII)I");
                m_availableId = jni->GetMethodID(inputStreamClass, "available", "()I");
                m_markId = jni->GetMethodID(inputStreamClass, "mark", "(I)V");
                m_skipId = jni->GetMethodID(inputStreamClass, "skip", "(J)J");
                m_resetId = jni->GetMethodID(inputStreamClass, "reset", "()V");
            }

            virtual ~AssetManagerJava()
            {
                JniEnv jni;
                jni->DeleteGlobalRef(m_assetMgr);
            }

            virtual AAsset* open(const char* filename, int mode, void* assetBuf)
            {
                JniEnv jni;

                jstring filenameStr = jni->NewStringUTF(filename);
                jobject stream = jni->CallObjectMethod(m_assetMgr, m_openId, filenameStr, mode);

                if (jni->ExceptionCheck())
                {
                    jni->ExceptionClear();
                    return NULL;
                }

                // mark at 0
                jni->CallVoidMethod(stream, m_markId, CK_INT32_MAX);

                AAsset* asset = (AAsset*) assetBuf;
                asset->m_stream = jni->NewGlobalRef(stream);
                asset->m_size = jni->CallIntMethod(stream, m_availableId);
                asset->m_pos = 0;
                jbyteArray array = jni->NewByteArray(k_bufSize);
                asset->m_buf = (jbyteArray) jni->NewGlobalRef(array);
                return asset;
            }

            virtual int read(AAsset* asset, void* buf, size_t count)
            {
                JniEnv jni;

                int totalBytesRead = 0;
                byte* p = (byte*) buf;
                while (totalBytesRead < count)
                {
                    int bytesToRead = Math::min((int)k_bufSize, (int)count - totalBytesRead);
                    int bytesRead = jni->CallIntMethod(asset->m_stream, m_readId, asset->m_buf, 0, bytesToRead);
                    if (bytesRead < 0)
                    {
                        break;
                    }

                    asset->m_pos += bytesRead;
                    totalBytesRead += bytesRead;

                    jbyte* byteBuf = jni->GetByteArrayElements(asset->m_buf, NULL);
                    Mem::copy(p, byteBuf, bytesRead);
                    jni->ReleaseByteArrayElements(asset->m_buf, byteBuf, JNI_ABORT);
                    p += bytesRead;

                    if (bytesRead < bytesToRead)
                    {
                        break;
                    }
                }

                return totalBytesRead;
            }

            virtual off_t seek(AAsset* asset, off_t offset)
            {
                // not bothering with whence param

                JniEnv jni;

                jlong skip = 0;
                if (offset > asset->m_pos)
                {
                    skip = offset - asset->m_pos;
                }
                else if (offset < asset->m_pos)
                {
                    jni->CallVoidMethod(asset->m_stream, m_resetId);
                    skip = offset;
                    asset->m_pos = 0;
                }

                for (;;)
                {
                    jlong skipped = jni->CallLongMethod(asset->m_stream, m_skipId, skip);
                    skip -= skipped;
                    asset->m_pos += skipped;
                    if (skipped <= 0 || skip <= 0)
                    {
                        break;
                    }
                }

                return asset->m_pos;
            }

            virtual off_t getLength(AAsset* asset)
            {
                return asset->m_size;
            }

            virtual int openFileDescriptor(AAsset* asset, off_t* start, off_t* length)
            {
                CK_FAIL("not implemented in java");
                return 0;
            }

            virtual void close(AAsset* asset)
            {
                JniEnv jni;
                jni->DeleteGlobalRef(asset->m_stream);
                jni->DeleteGlobalRef(asset->m_buf);
            }

        private:
            jobject m_assetMgr;
            jmethodID m_openId;
            jmethodID m_readId;
            jmethodID m_availableId;
            jmethodID m_markId;
            jmethodID m_skipId;
            jmethodID m_resetId;
        };

        ////////////////////////////////////////

        AssetManagerBase* g_mgr = NULL;
        byte g_mgrMem[Math::Max<sizeof(AssetManagerNative), sizeof(AssetManagerJava)>::value];

    }


    void init(jobject context)
    {
        JniEnv jni;
        jclass contextClass = jni->GetObjectClass(context);

        // assetMgr = context.getAssets();
        jmethodID methodId = jni->GetMethodID(contextClass, "getAssets", "()Landroid/content/res/AssetManager;");
        jobject assetMgr = jni->CallObjectMethod(context, methodId);

#if USE_JAVA_ASSETMGR
        g_mgr = new (g_mgrMem) AssetManagerJava(assetMgr);
#else
        if (SystemAndroid::get()->getSdkVersion() >= 9)
        {
            g_mgr = new (g_mgrMem) AssetManagerNative(assetMgr);
        }
        else
        {
            g_mgr = new (g_mgrMem) AssetManagerJava(assetMgr);
        }
#endif
    }

    void shutdown()
    {
        g_mgr->~AssetManagerBase();
        g_mgr = NULL;
    }

    AAsset* open(const char* filename, int mode, void* assetBuf)
    {
        return g_mgr->open(filename, mode, assetBuf);
    }

    int read(AAsset* asset, void* buf, size_t count)
    {
        return g_mgr->read(asset, buf, count);
    }

    off_t seek(AAsset* asset, off_t offset)
    {
        return g_mgr->seek(asset, offset);
    }

    off_t getLength(AAsset* asset)
    {
        return g_mgr->getLength(asset);
    }

    int openFileDescriptor(AAsset* asset, off_t* start, off_t* length)
    {
        return g_mgr->openFileDescriptor(asset, start, length);
    }

    void close(AAsset* asset)
    {
        g_mgr->close(asset);
    }
}

}
