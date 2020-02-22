#include "ck/audio/sourcenode.h"
#include "ck/audio/audiosource.h"
#include "ck/audio/audioutil.h"
#include "ck/audio/decoder.h"
#include "ck/audio/audiograph.h"
#include "ck/audio/mixnode.h"
#include "ck/core/debug.h"
#include "ck/core/mem.h"
#include "ck/core/math.h"
#include "ck/core/fixedpoint.h"


namespace Cki
{


SourceNode::SourceNode() :
    m_source(NULL),
    m_decoder(NULL),
    m_next(NULL),
    m_output(NULL),
    m_connected(false),
    m_playId(0),
    m_doneId(0),
    m_outOffset(0),
    m_paused(false),
    m_nextFrame(-1),
    m_nextMs(-1.0f),
    m_speed(1.0f)
{
    m_volMat.setIdentity();
    m_targetVolMat.setIdentity();
}

void SourceNode::init(AudioSource* source)
{
    CK_ASSERT(!m_source);
    m_source = source;
}

BufferHandle SourceNode::processFloat(int frames, bool post, bool& needsPost)
{
    return process<float>(frames, post, needsPost);
}

BufferHandle SourceNode::processFixed(int frames, bool post, bool& needsPost)
{
    return process<int32>(frames, post, needsPost);
}

template <typename T>
BufferHandle SourceNode::process(int outFramesReq, bool post, bool& needsPost)
{
    int outOffset = 0;
    if (post)
    {
        if (m_outOffset < 0)
        {
            return BufferHandle(); // no post-process needed
        }
        outOffset = m_outOffset;
    }
    m_outOffset = -1;

    outFramesReq -= outOffset;

    if (!m_source->isInited())
    {
        return BufferHandle(); // inactive
    }

    const SampleInfo& sampleInfo = m_source->getSampleInfo();

    // seek if needed
    int nextFrame = -1;
    if (m_nextFrame >= 0)
    {
        nextFrame = m_nextFrame;
    }
    else if (m_nextMs >= 0.0f)
    {
        nextFrame = Math::round(m_nextMs * 0.001f * sampleInfo.sampleRate);
    }

    if (nextFrame >= 0)
    {
        m_decoder->setFramePos(nextFrame);
        m_nextFrame = -1;
        m_nextMs = -1.0f;
    }

    bool ready = m_source->isReady();

    if (!m_playId || m_paused || !ready || m_speed <= 0.0f)
    {
        return BufferHandle(); // inactive
    }


    float factor = m_speed * sampleInfo.sampleRate / getSampleRate();
    int inFramesReq = Math::round(outFramesReq * factor);

    // TODO: if inFrames > maxFrames, process multiple chunks
    int maxFrames = getBufferFrames();
    if (outOffset + inFramesReq > maxFrames)
    {
        inFramesReq = maxFrames - outOffset;
        factor = (float) inFramesReq / outFramesReq;
    }

    BufferHandle inputHandle = acquireBuffer();
    T* inBuf = (T*) inputHandle.get();

    // copy input data
    int inFrames = m_decoder->decode(inBuf + outOffset * sampleInfo.channels, inFramesReq);
    CK_ASSERT(inFrames <= inFramesReq);
    int outFrames;
    if (inFrames < inFramesReq)
    {
        outFrames = Math::round(inFrames / factor);
    }
    else
    {
        outFrames = outFramesReq;
    }

    // resample 
    BufferHandle resampledHandle;
    T* resampledBuf = NULL;
    if (inFrames == outFrames)
    {
        resampledHandle = inputHandle;
        resampledBuf = (T*) resampledHandle.get();
    }
    else
    {
        resampledHandle = acquireBuffer();
        resampledBuf = (T*) resampledHandle.get();
        m_resampler.resample(inBuf + outOffset * sampleInfo.channels, inFrames, resampledBuf + outOffset * sampleInfo.channels, outFrames, sampleInfo.channels);
    }

    inputHandle.release();

    // pan and convert to stereo
    BufferHandle outputHandle;
    T* outputBuf = NULL;
    if (sampleInfo.channels == 1)
    {
        outputHandle = acquireBuffer();
        outputBuf = (T*) outputHandle.get();
        AudioUtil::monoPan(resampledBuf + outOffset, outputBuf + outOffset * 2, outFrames, m_targetVolMat, m_volMat, getVolumeRate());
    }
    else
    {
        outputHandle = resampledHandle;
        outputBuf = (T*) outputHandle.get();
        AudioUtil::stereoPan(outputBuf + outOffset * 2, outFrames, m_targetVolMat, m_volMat, getVolumeRate());
    }

    if (outOffset > 0)
    {
        // fill empty intro with zeroes
        Mem::clear(outputBuf, outOffset * k_maxChannels * sizeof(int32));
    }

    if (outFrames < outFramesReq)
    {
        // fill leftover space with zeroes
        Mem::clear(outputBuf + (outFrames + outOffset) * k_maxChannels, (outFramesReq - outFrames) * sizeof(int32) * k_maxChannels);
    }

    if (m_decoder->isDone())
    {
        if (m_next)
        {
            m_next->m_playId = -1;
            m_next->connect();
            m_next->m_volMat = m_next->m_targetVolMat; 
            m_next->m_resampler.copyState(m_resampler);
            if (outFrames < outFramesReq)
            {
                needsPost = true;
                m_next->m_outOffset = outOffset + outFrames;
            }
        }

        m_source->reset();
        if (!(m_next == this && m_playId == -1))
        {
            // can't call disconnect() directly, because output is currently iterating through its inputs
            AudioGraph::get()->execute(this, k_disconnect);

            m_doneId = m_playId;
            m_playId = 0;
            m_resampler.resetState();
        }
    }

    return outputHandle;
}

void SourceNode::execute(int cmdId, CommandParam param0, CommandParam param1)
{
    switch (cmdId)
    {
        case k_setLoopCount:
            m_source->setLoopCount(param0.intValue);
            break;

        case k_releaseLoop:
            m_source->releaseLoop();
            break;

        case k_setLoop:
            m_source->setLoop(param0.intValue, param1.intValue);
            break;

        case k_setNextFrame:
            m_decoder->setFramePos(param0.intValue);
            break;

        case k_setDecoder:
            m_decoder = (Decoder*) param0.addrValue;
            break;

        case k_setOutput:
            {
                MixNode* output = (MixNode*) param0.addrValue;
                if (output != m_output)
                {
                    if (m_connected)
                    {
                        m_output->removeInput(this);
                    }
                    m_output = output;
                    if (m_connected)
                    {
                        m_output->addInput(this);
                    }
                }
            }
            break;

        case k_connect:
            connect();
            break;

        case k_disconnect:
            disconnect();
            break;

        case k_setNext:
            m_next = (SourceNode*) param0.addrValue;
            break;

        case k_setSpeed:
            m_speed = param0.floatValue;
            CK_ASSERT(m_speed >= 0.0f);
            break;

        case k_setVolumeL:
            m_targetVolMat.ll = param0.floatValue;
            m_targetVolMat.lr = param1.floatValue;
            break;

        case k_setVolumeR:
            m_targetVolMat.rl = param0.floatValue;
            m_targetVolMat.rr = param1.floatValue;
            break;

        case k_setPlaying:
            {
                int playId = param0.intValue;
                if (m_playId != playId)
                {
                    if (!playId)
                    {
                        // stopping, so reset for next time
                        m_source->reset();
                        disconnect();
                    }
                    m_volMat = m_targetVolMat; // reset volume
                    m_playId = playId;
                    m_resampler.resetState();
                }
            }
            break;

        case k_setPaused:
            {
                bool paused = param0.boolValue();
                if (m_paused != paused)
                {
                    m_paused = paused;
                    m_volMat = m_targetVolMat; // reset volume
                }
            }
            break;

        case k_setPlayPosition:
            m_nextFrame = param0.intValue;
            m_nextMs = -1.0f;
            break;

        case k_setPlayPositionMs:
            m_nextMs = param0.floatValue;
            m_nextFrame = -1;
            break;

        default:
            CK_FAIL("unknown command");
    };
}

#if CK_DEBUG
void SourceNode::print(int level)
{
    printImpl(level, "source");
}
#endif

void SourceNode::disconnect()
{
    if (m_connected)
    {
        m_output->removeInput(this);
        m_connected = false;
    }
}

void SourceNode::connect()
{
    if (!m_connected)
    {
        m_output->addInput(this);
        m_connected = true;
    }
}


}
