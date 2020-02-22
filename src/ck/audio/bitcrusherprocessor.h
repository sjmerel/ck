#pragma once

#include "ck/core/platform.h"
#include "ck/core/types.h"
#include "ck/audio/effectprocessor.h"
#include "ck/audio/varsample.h"

namespace Cki
{


class BitCrusherProcessor : public EffectProcessor
{
public:
    BitCrusherProcessor();

    virtual void setParam(int paramId, float value);
    virtual void reset();
    virtual bool isInPlace() const { return true; }

    virtual void process(int* inBuf, int* outBuf, int frames);
    virtual void process(float* inBuf, float* outBuf, int frames);

    virtual void process_default(int* inBuf, int* outBuf, int frames);
    virtual void process_neon(int* inBuf, int* outBuf, int frames);
    virtual void process_sse(int* inBuf, int* outBuf, int frames);
    virtual void process_default(float* inBuf, float* outBuf, int frames);
    virtual void process_neon(float* inBuf, float* outBuf, int frames);
    virtual void process_sse(float* inBuf, float* outBuf, int frames);

private:
    int m_bits;
    float m_holdMs;
    VarSample m_holdValue[2];
    int m_holdFramesLeft; // frames left to hold
};


}

