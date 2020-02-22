#pragma once

#include "ck/audio/graphoutput.h"
#include "ck/core/thread.h"
#include <jni.h>

namespace Cki
{

class AudioTrackProxy;

class GraphOutputJavaAndroid : public GraphOutput
{
public:
    GraphOutputJavaAndroid();
    virtual ~GraphOutputJavaAndroid();

protected:
    virtual void startImpl();
    virtual void stopImpl();

private:
    AudioTrackProxy* m_audioTrack;
    jshortArray m_data;
    int32* m_renderBuf;
    float m_bufferMs;
    int m_dataFrames; // frames in a slice
    int m_bufFrames; // frames in AudioTrack buffer
    int m_writeCount;
    int m_readPos;
    int m_readCount;
    Thread m_updateThread;
    volatile bool m_updateThreadDone;

    static void* updateThreadFunc(void*);
    void updateLoop();
    void initBuffer();
    void renderBuffer();
};


}


