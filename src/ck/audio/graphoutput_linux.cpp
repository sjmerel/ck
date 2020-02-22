#include "ck/audio/graphoutput_linux.h"
#include "ck/audio/audiodebug_linux.h"
#include "ck/audio/audioutil.h"
#include "ck/core/debug.h"
#include "ck/core/mem.h"
#include "ck/core/system.h"
#include "ck/core/logger.h"

namespace Cki
{


GraphOutputLinux::GraphOutputLinux() :
    m_handle(0),
    m_thread(threadFunc),
    m_stop(false),
    m_buf(NULL),
    m_bufFrames(0)
{
    snd_pcm_hw_params_t *hw_params;
    CK_ALSA_VERIFY( snd_pcm_open(&m_handle, "default", SND_PCM_STREAM_PLAYBACK, 0) );
    CK_ALSA_VERIFY( snd_pcm_hw_params_malloc(&hw_params) );
    CK_ALSA_VERIFY( snd_pcm_hw_params_any(m_handle, hw_params) );
    CK_ALSA_VERIFY( snd_pcm_hw_params_set_access(m_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED) );
    CK_ALSA_VERIFY( snd_pcm_hw_params_set_format(m_handle, hw_params, SND_PCM_FORMAT_FLOAT_LE) );
    CK_ALSA_VERIFY( snd_pcm_hw_params_set_rate(m_handle, hw_params, AudioNode::k_maxSampleRate, 0) );
    CK_ALSA_VERIFY( snd_pcm_hw_params_set_channels(m_handle, hw_params, 2) );

    float updateMs = System::get()->getConfig().audioUpdateMs;
    int updateFrames = (int) (AudioNode::k_maxSampleRate * updateMs * 0.001f);

    snd_pcm_uframes_t bufferFrames = updateFrames*2;
    CK_ALSA_VERIFY( snd_pcm_hw_params_set_buffer_size_near(m_handle, hw_params, &bufferFrames) );

    m_bufFrames = bufferFrames/2;
    if (m_bufFrames != updateFrames)
    {
        CK_LOG_INFO("buffer duration is %f ms (desired %f)", 
                1000.0f * m_bufFrames / AudioNode::k_maxSampleRate,
                1000.0f * updateFrames / AudioNode::k_maxSampleRate);
    }

    m_buf = (float*) Mem::alloc(m_bufFrames * sizeof(float) * AudioNode::k_maxChannels, 16);

    CK_ALSA_VERIFY( snd_pcm_hw_params(m_handle, hw_params) );

    /*
    int dir = 0;
    snd_pcm_uframes_t period = 0;
    CK_ALSA_VERIFY( snd_pcm_hw_params_get_period_size(hw_params, &period, &dir) );
    CK_PRINT("period size: %d\n", period);
    snd_pcm_uframes_t buffer = 0;
    CK_ALSA_VERIFY( snd_pcm_hw_params_get_buffer_size(hw_params, &buffer) );
    CK_PRINT("buffer size: %d\n", buffer);
    */

    snd_pcm_hw_params_free(hw_params);

    snd_pcm_sw_params_t *sw_params;
    CK_ALSA_VERIFY( snd_pcm_sw_params_malloc(&sw_params) );
    CK_ALSA_VERIFY( snd_pcm_sw_params_current(m_handle, sw_params) );
    CK_ALSA_VERIFY( snd_pcm_sw_params_set_start_threshold(m_handle, sw_params, CK_UINT32_MAX) ); // start explicitly
    CK_ALSA_VERIFY( snd_pcm_sw_params(m_handle, sw_params) );
    snd_pcm_sw_params_free(sw_params);

    System::get()->getConfig().audioUpdateMs = 1000.0f * m_bufFrames / AudioNode::k_maxSampleRate;
}

GraphOutputLinux::~GraphOutputLinux()
{
    m_stop = true;
    snd_pcm_close(m_handle);
    m_thread.join();
    Mem::free(m_buf);
}

void GraphOutputLinux::startImpl()
{
    CK_ALSA_VERIFY( snd_pcm_prepare(m_handle) );
    m_stop = false;
    m_thread.start(this);
}

void GraphOutputLinux::stopImpl()
{
    m_stop = true;
    m_thread.join();
    CK_ALSA_VERIFY( snd_pcm_drain(m_handle) );
}

void GraphOutputLinux::threadLoop()
{
    CK_ALSA_VERIFY( snd_pcm_wait(m_handle, -1) );

    int bufPos = 0;
    bool running = false;
    while (!m_stop)
    {
        if (bufPos == 0)
        {
            if (System::get()->getSampleType() == kCkSampleType_Float)
            {
                render((float*) m_buf, m_bufFrames);
            }
            else
            {
                render((int32*) m_buf, m_bufFrames);
                AudioUtil::convert((const int32*) m_buf, (float*) m_buf, m_bufFrames);
            }
        }

        int result = snd_pcm_writei(m_handle, m_buf + bufPos, m_bufFrames - bufPos);
        if (result > 0)
        {
            bufPos += result;
            CK_ASSERT(bufPos <= m_bufFrames);
            if (bufPos >= m_bufFrames)
            {
                bufPos = 0;
            }

            if (!running)
            {
                // XXX we sometimes get error -77 here, "File descriptor in bad state";
                // only reproduced so far in x86 debug builds.  The stream appears to be 
                // started though, so ignoring it...
                int err = snd_pcm_start(m_handle);
                CK_UNUSED(err);
                CK_ASSERT(err >= 0 || err == -77, "%s (%d)", snd_strerror(err), err);
                running = true;
            }
        }
        else //if (result == -EPIPE)
        {
            CK_ALSA_VERIFY( snd_pcm_prepare(m_handle) );
            running = false;
        }
    }
}

void* GraphOutputLinux::threadFunc(void* arg)
{
    GraphOutputLinux* me = (GraphOutputLinux*) arg;
    me->threadLoop();
    return NULL;
}


}


