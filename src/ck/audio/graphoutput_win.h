#pragma once

#include "ck/audio/graphoutput.h"


namespace Cki
{


class GraphOutputWin : public GraphOutput
{
public:
    GraphOutputWin();
    virtual ~GraphOutputWin();

protected:
    virtual void startImpl();
    virtual void stopImpl();

private:
    float* m_bufMem;
    float* m_bufs[2];
    int m_bufSize;
    int m_curBuf;

    void submitBuffer(bool render);
    static void bufferEnd(void* data);
};


}


