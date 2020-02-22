#pragma once

#include "ck/core/platform.h"
#include "ck/core/types.h"
#include "ck/audio/effectprocessor.h"

namespace Cki
{

class DistortionProcessor : public EffectProcessor
{
public:
    DistortionProcessor();

    virtual void setParam(int paramId, float value);
    virtual void reset();
    virtual bool isInPlace() const { return true; }

    virtual void process_default(int* inBuf, int* outBuf, int frames);
    virtual void process_neon(int* inBuf, int* outBuf, int frames);
    virtual void process_sse(int* inBuf, int* outBuf, int frames);
    virtual void process_default(float* inBuf, float* outBuf, int frames);
    virtual void process_neon(float* inBuf, float* outBuf, int frames);
    virtual void process_sse(float* inBuf, float* outBuf, int frames);

private:
    float m_drive;
    float m_offset;
};


}


