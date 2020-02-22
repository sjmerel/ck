#pragma once

#include "ck/audio/graphoutput.h"
#include "ck/core/thread.h"
#include <alsa/asoundlib.h>


namespace Cki
{


class GraphOutputLinux : public GraphOutput
{
public:
    GraphOutputLinux();
    virtual ~GraphOutputLinux();

protected:
    virtual void startImpl();
    virtual void stopImpl();

private:
    snd_pcm_t* m_handle;
    Thread m_thread;
    bool m_stop;
    float* m_buf;
    int m_bufFrames;

    void threadLoop();
    static void* threadFunc(void*);
};


}



