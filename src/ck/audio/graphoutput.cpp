#include "ck/audio/graphoutput.h"
#include "ck/audio/audioutil.h"
#include "ck/core/debug.h"
#include "ck/core/fixedpoint.h"
#include "ck/core/mem.h"
#include "ck/core/math.h"
#include "ck/core/logger.h"
#include "ck/core/system.h"
#include "ck/core/mutexlock.h"
#include "ck/core/dir.h"
#include "ck/core/system_platform.h"
#include "ck/audio/rawwriter.h"
#include "ck/audio/wavwriter.h"

//#include "ck/core/profiler.h"

#if CK_PLATFORM_IOS || CK_PLATFORM_OSX || CK_PLATFORM_TVOS
#  include "ck/audio/graphoutput_ios.h"
#elif CK_PLATFORM_ANDROID
#  include "ck/audio/graphoutput_android.h"
#  include "ck/audio/graphoutputjava_android.h"
#elif CK_PLATFORM_LINUX
#  include "ck/audio/graphoutput_linux.h"
#elif CK_PLATFORM_WIN || CK_PLATFORM_WP8
#  include "ck/audio/graphoutput_win.h"
#endif
#include "ck/audio/graphoutput_dummy.h"


namespace Cki
{


GraphOutput* GraphOutput::create()
{
    if (System::get()->isToolMode())
    {
        return new GraphOutputDummy(true);
    }
    else
    {
#if CK_PLATFORM_IOS || CK_PLATFORM_OSX || CK_PLATFORM_TVOS
        return new GraphOutputIos();
#elif CK_PLATFORM_LINUX
        return new GraphOutputLinux();
#elif CK_PLATFORM_ANDROID
        SystemAndroid* system = SystemAndroid::get();
        if (system->getSdkVersion() >= 9 && !system->getConfig().useJavaAudio)
        {
            return new GraphOutputAndroid();
        }
        else
        {
            return new GraphOutputJavaAndroid();
        }
#elif CK_PLATFORM_WIN || CK_PLATFORM_WP8
        return new GraphOutputWin();
#endif
    }
}


GraphOutput::GraphOutput() :
    m_fixedPoint(System::get()->getSampleType() == kCkSampleType_Fixed),
    m_input(NULL),
    m_playing(false),
    m_clip(false),
    m_captureWriter(NULL),
    m_renderFrac(0.0f),
    m_renderAvg(m_renderAvgBuf, k_avgCount),
    m_frameAvg(m_frameAvgBuf, k_avgCount)
{
}

GraphOutput::~GraphOutput()
{
    delete m_captureWriter;
}

void GraphOutput::setInput(AudioNode* input)
{
    m_input = input;
}

void GraphOutput::setPreRenderCallback(Callback0::Func cb, void* data)
{
    m_preRenderCallback.set(cb, data);
}

void GraphOutput::setPostRenderCallback(Callback0::Func cb, void* data)
{
    m_postRenderCallback.set(cb, data);
}

void GraphOutput::start()
{
    if (!m_playing)
    {
        startImpl();
        m_playing = true;
    }
}

void GraphOutput::stop()
{
    if (m_playing)
    {
        stopImpl();
        m_playing = false;
        m_renderFrac = 0.0f;
        m_renderAvg.reset();
        m_frameAvg.reset();

        m_frameTimer.stop();
        m_frameTimer.reset();
        m_renderTimer.stop();
        m_renderTimer.reset();
    }
}

bool GraphOutput::isRunning() const
{
    return m_playing;
}

//Profiler prof("render", 100);

template <typename T>
bool GraphOutput::render(T* buf, int frames)
{
    m_frameAvg.sample(m_frameTimer.getElapsedMs());
    m_renderAvg.sample(m_renderTimer.getElapsedMs());

    m_renderFrac = m_renderAvg.getSum() / m_frameAvg.getSum();

    m_frameTimer.reset();
    m_renderTimer.reset();

    m_frameTimer.start();
    m_renderTimer.start();

    const int k_numChannels = AudioNode::k_maxChannels;

//prof.start();

    m_preRenderCallback.call();

#if CK_DEBUG
    /*
    if (m_input)
    {
        m_input->print(0);
    }
    */
#endif

    BufferHandle inHandle;

    // if we can't fit all the data in the buffer (which should only happen rarely),
    // split it up in similar-sized chunks
    int divs = 1;
    while (frames/divs+1 > AudioNode::getBufferFrames())
    {
        ++divs;
    }

    while (frames > 0)
    {
        int framesToRender = Math::min(frames/divs, AudioNode::getBufferFrames());
        if (divs > 1)
        {
            --divs;
        }

        if (m_input)
        {
            bool post = false;
            inHandle = m_input->process<T>(framesToRender, false, post);
            if (inHandle.isValid())
            {
                while (post)
                {
                    // post-process
                    post = false;
                    BufferHandle nextHandle = m_input->process<T>(framesToRender, true, post);
                    if (nextHandle.isValid())
                    {
                        AudioUtil::mix((const T*) nextHandle.get(), (T*) inHandle.get(), k_numChannels * framesToRender);
                    }
                }
                m_clip |= AudioUtil::clamp((const T*) inHandle.get(), buf, k_numChannels * framesToRender);
            }
        }

        if (!inHandle.isValid())
        {
            Mem::clear(buf, k_numChannels * frames * sizeof(T));
        }

        {
            MutexLock lock(m_captureMutex);
            if (m_captureWriter)
            {
                int samples = k_numChannels * frames;
                if (m_captureWriter->write(buf, samples) != samples)
                {
                    CK_LOG_ERROR("Capture failed; closing file");
                    delete m_captureWriter;
                    m_captureWriter = NULL;
                }
            }
        }

        frames -= framesToRender;
        buf += framesToRender * k_numChannels;
    }

    m_postRenderCallback.call();
// prof.stop();

    m_renderTimer.stop();

    return inHandle.isValid();
}

void GraphOutput::startCapture(const char* path, CkPathType pathType)
{
    stopCapture();

    MutexLock lock(m_captureMutex);

#if CK_PLATFORM_ANDROID
    if (pathType == kCkPathType_Asset)
    {
        CK_LOG_ERROR("Cannot capture to path of type kCkPathType_Asset; assets are read-only.");
        return;
    }
#endif
#if CK_PLATFORM_WP8
    if (pathType == kCkPathType_InstallationDir)
    {
        CK_LOG_ERROR("Cannot capture to path of type kCkPathType_InstallationDir; installation folder is read-only.");
        return;
    }
#endif
#if CK_PLATFORM_IOS || CK_PLATFORM_TVOS
    if (pathType == kCkPathType_Resource)
    {
        CK_LOG_ERROR("Cannot capture to path of type kCkPathType_Resource; resource folder is read-only.");
        return;
    }
#endif

    Path outPath(path, pathType);

    Path dirPath(outPath);
    dirPath.setParent();
    if (!Dir::exists(dirPath.getBuffer()) && !Dir::create(dirPath.getBuffer()))
    {
        CK_LOG_ERROR("Could not create directory %s for capture", dirPath.getBuffer());
        return;
    }

    AudioWriter* writer = NULL;
    if (outPath.hasExtension("raw"))
    {
        writer = new RawWriter(outPath.getBuffer(), m_fixedPoint);
    }
    else if (outPath.hasExtension("wav"))
    {
        writer = new WavWriter(outPath.getBuffer(), AudioNode::k_maxChannels, AudioNode::getSampleRate(), m_fixedPoint);
    }
    else
    {
        CK_LOG_ERROR("Cannot capture to %s; must have extension .raw or .wav", path);
        return;
    }

    if (!writer->isValid())
    {
        CK_LOG_ERROR("Could not open file %s for capture", outPath.getBuffer());
        delete writer;
        return;
    }

    CK_LOG_INFO("Capturing to %s", outPath.getBuffer());
    m_captureWriter = writer;
}

void GraphOutput::stopCapture()
{
    MutexLock lock(m_captureMutex);
    if (m_captureWriter)
    {
        CK_LOG_INFO("Capture stopped");
        delete m_captureWriter;
        m_captureWriter = NULL;
    }
}

template bool GraphOutput::render<float>(float* buf, int frames);
template bool GraphOutput::render<int32>(int32* buf, int frames);


}
