#pragma once

#include "ck/audio/graphoutput.h"


namespace Cki
{

class Thread;

class GraphOutputDummy : public GraphOutput
{
public:
    GraphOutputDummy(bool polled);
    virtual ~GraphOutputDummy();

    static void poll();

protected:
    virtual void startImpl();
    virtual void stopImpl();

private:
    void* m_buf;
    Thread* m_thread;
    bool m_stopThread;
    static GraphOutputDummy* s_inst;

    void threadLoop();
    void doRender();
    static void* threadFunc(void* arg);
};


}



