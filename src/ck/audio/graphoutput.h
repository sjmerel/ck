#pragma once

#include "ck/ck.h"
#include "ck/core/platform.h"
#include "ck/core/callback.h"
#include "ck/core/allocatable.h"
#include "ck/core/mutex.h"
#include "ck/core/timer.h"
#include "ck/core/runningaverage.h"
#include "ck/audio/audionode.h"

namespace Cki
{

class AudioWriter;

class GraphOutput : public Allocatable
{
public:
    virtual ~GraphOutput();
    static GraphOutput* create();

    void setInput(AudioNode*);

    void setPreRenderCallback(Callback0::Func, void* data);
    void setPostRenderCallback(Callback0::Func, void* data);

    void start();
    void stop();
    bool isRunning() const;

    bool getClipFlag() const { return m_clip; }
    void resetClipFlag() { m_clip = false; }

    float getRenderLoad() const { return m_renderFrac; }
    float getRenderMs() const { return m_renderTimer.getElapsedMs(); }

    void startCapture(const char* path, CkPathType);
    void stopCapture();

protected:
    template <typename T>
    bool render(T* buf, int frames);
    bool m_fixedPoint;

    GraphOutput();

    virtual void startImpl() = 0;
    virtual void stopImpl() = 0;

private:
    AudioNode* m_input;
    Callback0 m_preRenderCallback;
    Callback0 m_postRenderCallback;
    bool m_playing;
    bool m_clip;
    AudioWriter* m_captureWriter;
    Mutex m_captureMutex;

    // profiling:
    Timer m_renderTimer;
    Timer m_frameTimer;
    float m_renderFrac;

    enum { k_avgCount = 50 };
    float m_renderAvgBuf[k_avgCount];
    RunningAverage m_renderAvg;
    float m_frameAvgBuf[k_avgCount];
    RunningAverage m_frameAvg;
};


}

