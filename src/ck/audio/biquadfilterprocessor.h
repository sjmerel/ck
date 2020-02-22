#pragma once

#include "ck/core/platform.h"
#include "ck/core/types.h"
#include "ck/audio/effectprocessor.h"
#include "ck/audio/varsample.h"

namespace Cki
{

class BiquadFilterProcessor : public EffectProcessor
{
public:
    BiquadFilterProcessor();

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
    int m_type;
    float m_freq;
    float m_q;
    float m_gain;
    bool m_calc;

    VarSample m_b0a0;
    VarSample m_b1a0;
    VarSample m_b2a0;
    VarSample m_a1a0;
    VarSample m_a2a0;

    // history
    VarSample m_x[2][2];
    VarSample m_y[2][2];

    void calcCoeffs(bool fixedPoint);
};


}

