#pragma once

#include "ck/core/platform.h"
#include "ck/core/allocatable.h"
#include "ck/effectprocessor.h"

namespace Cki
{

class EffectProcessor : public CkEffectProcessor, public Allocatable 
{
public:
    virtual void process(int* inBuf, int* outBuf, int frames);
    virtual void process(float* inBuf, float* outBuf, int frames);

    // these exposed just for dsptest:
    virtual void process_default(int* inBuf, int* outBuf, int frames) = 0;
    virtual void process_neon(int* inBuf, int* outBuf, int frames) = 0;
    virtual void process_sse(int* inBuf, int* outBuf, int frames) = 0;
    virtual void process_default(float* inBuf, float* outBuf, int frames) = 0;
    virtual void process_neon(float* inBuf, float* outBuf, int frames) = 0;
    virtual void process_sse(float* inBuf, float* outBuf, int frames) = 0;

private:
    template <typename T> void processImpl(T* inBuf, T* outBuf, int frames);
};

}


