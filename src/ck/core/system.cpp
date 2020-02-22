#include "ck/core/system.h"
#include "ck/core/debug.h"
#include "ck/core/mem.h"
#include "ck/core/timer.h"
#include "ck/core/logger.h"
#include "ck/core/version.h"
#include "ck/core/profiler.h"
#include "ck/core/asyncloader.h"
#include "ck/core/deletable.h"
#include "ck/core/system_platform.h"
#include "ck/audio/audio.h"
#include "ck/audio/audiograph.h"
#include <time.h>

#if CK_PLATFORM_ANDROID
#  include "ck/core/assetmanager_android.h"
#  include <cpu-features.h>
#endif



namespace Cki
{


bool System::init(CkConfig& config, bool toolMode)
{
    bool success = true;
    if (!get())
    {
        if (!toolMode)
        {
            CK_PRINT("\n");
            CK_PRINT("Using Cricket Audio %d.%d.%d%s%s from Cricket Technology (www.crickettechnology.com)\n", Version::k_major, Version::k_minor, Version::k_build, (strlen(Version::k_label) ? " " : ""), Version::k_label);
            CK_PRINT("\n");
            CK_PRINT("If you have purchased a source code license from Cricket Technology, this product\n");
            CK_PRINT("  is covered by the source code license\n");
            CK_PRINT("  (http://www.crickettechnology.com/source_license).\n");
            CK_PRINT("Otherwise, this product is covered by the free license\n");
            CK_PRINT("  (http://www.crickettechnology.com/free_license).\n");
            CK_PRINT("\n");
        }

        g_logger.setMask(config.logMask);
        g_logger.setFunc(config.logFunc);

        // check config
        if (config.audioUpdateMs <= 0.0f)
        {
            CK_LOG_ERROR("Config.audioUpdateMs set to invalid value (%f); setting to default (%f) instead.", config.audioUpdateMs, CkConfig_audioUpdateMsDefault);
            config.audioUpdateMs = CkConfig_audioUpdateMsDefault;
        }
        if (config.streamBufferMs <= 0.0f)
        {
            CK_LOG_ERROR("Config.streamBufferMs set to invalid value (%f); setting to default (%f) instead.", config.streamBufferMs, CkConfig_streamBufferMsDefault);
            config.streamBufferMs = CkConfig_streamBufferMsDefault;
        }
        if (config.streamFileUpdateMs <= 0.0f)
        {
            CK_LOG_ERROR("Config.streamFileUpdateMs set to invalid value (%f); setting to default (%f) instead.", config.streamFileUpdateMs, CkConfig_streamFileUpdateMsDefault);
            config.streamFileUpdateMs = CkConfig_streamFileUpdateMsDefault;
        }
        if (config.maxAudioTasks < 10)
        {
            CK_LOG_ERROR("Config.maxAudioTasks set to invalid value (%d); setting to default (%d) instead.", config.maxAudioTasks, CkConfig_maxAudioTasksDefault);
            config.maxAudioTasks = CkConfig_maxAudioTasksDefault;
        }

        SystemPlatform::init(config);

        System* system = get();
        if (config.sampleType == kCkSampleType_Default)
        {
#if CK_PLATFORM_ANDROID && __ARM_ARCH_5TE__
            // we're not compiling for VFP, so float math is done in software even if VFP is present
            system->m_sampleType = kCkSampleType_Fixed;
#else
            system->m_sampleType = kCkSampleType_Float;
#endif
        }
        else
        {
            system->m_sampleType = config.sampleType;
        }

        CK_LOG_INFO("using %s pipeline %s",
                (system->m_sampleType == kCkSampleType_Float ? "floating-point" : "fixed-point"),
                (system->m_config.sampleType != kCkSampleType_Default ? "(overriding default)" : ""));
        CK_LOG_INFO("SIMD %s", system->m_simd ? "enabled" : "not supported");

        system->m_toolMode = toolMode;

        // these may depend on the above calls having already been made:
        success &= Audio::init();
        Deletable::init();
        Timer::init();
        AsyncLoader::init();

#if CK_PLATFORM_ANDROID
        AssetManager::init(config.context);
#endif
    }
    
    return success;
}

System* System::get()
{
    return SystemPlatform::get();
}

void System::shutdown()
{
    if (get())
    {
        CK_LOG_INFO("Cricket shutdown");

        AsyncLoader::shutdown();
        if (AudioGraph::get())
        {
            AudioGraph::get()->stop();
        }
        Deletable::shutdown();
        Audio::shutdown();
#if CK_PLATFORM_ANDROID
        AssetManager::shutdown();
#endif
        Mem::shutdown();

        SystemPlatform::shutdown();

        // sometimes required to get all console output on cygwin:
        fflush(stdout);
    }
}

CkConfig& System::getConfig()
{
    return m_config;
}

//static Profiler prof("system update", 100);
void System::update()
{
//    prof.start();
    Audio::update();
    Deletable::update();
//    prof.stop();
}

void System::suspend()
{
    if (!m_suspended)
    {
        Audio::suspend();
        m_suspended = true;
    }
}

void System::resume()
{
    if (m_suspended)
    {
        Audio::resume();
        m_suspended = false;
    }
}

System::System(const CkConfig& config) : 
    m_config(config),
    m_alloc(config.allocFunc, config.freeFunc),
    m_suspended(false),
    m_simd(false),
    m_toolMode(false),
    m_sampleType(kCkSampleType_Default)
{
    if (config.allocFunc && config.freeFunc)
    {
        Mem::init(&m_alloc);
    }
    else
    {
        Mem::init(NULL); // system allocator
    }

    printSysInfo();
}

System::~System()
{
}

void System::printSysInfo()
{
#if CK_PLATFORM_OSX
    const char* platform = "osx";
#elif CK_PLATFORM_TVOS
#  if TARGET_OS_SIMULATOR
    const char* platform = "tvos-simulator";
#  else
    const char* platform = "tvos";
#  endif
#elif CK_PLATFORM_WIN
    const char* platform = "windows";
#elif CK_PLATFORM_WP8
    const char* platform = "windows phone 8";
#elif CK_PLATFORM_IOS
#  if TARGET_OS_SIMULATOR
    const char* platform = "ios-simulator";
#  else
    const char* platform = "ios";
#  endif
#elif CK_PLATFORM_ANDROID
    const char* platform = "android";
#elif CK_PLATFORM_LINUX
    const char* platform = "linux";
#endif

    int ptrBits = sizeof(void*) * 8;

#if CK_DEBUG
    const char* config = "debug";
#else
    const char* config = "release";
#endif

#if CK_BIG_ENDIAN
    const char* byteOrder = "big-endian";
#else
    const char* byteOrder = "little-endian";
#endif

    CK_LOG_INFO("%s %d-bit %s %s", platform, ptrBits, config, byteOrder);
}



}
