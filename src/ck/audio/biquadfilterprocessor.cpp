#include "ck/audio/biquadfilterprocessor.h"
#include "ck/effect.h"
#include "ck/effectparam.h"
#include "ck/core/math.h"
#include "ck/core/debug.h"
#include "ck/core/fixedpoint.h"
#include "ck/core/logger.h"
#include "ck/audio/audioutil.h"

namespace Cki
{

// see http://www.musicdsp.org/files/Audio-EQ-Cookbook.txt

BiquadFilterProcessor::BiquadFilterProcessor() :
    m_type(kCkBiquadFilterParam_FilterType_LowPass),
    m_freq(2000.0f),
    m_q(1.0f),
    m_gain(1.0f),
    m_calc(true),
    m_b0a0(0),
    m_b1a0(0),
    m_b2a0(0),
    m_a1a0(0),
    m_a2a0(0)
{
    reset();
}

void BiquadFilterProcessor::reset()
{
    for (int i = 0; i < 2; ++i)
    {
        for (int chan = 0; chan < 2; ++chan)
        {
            m_x[i][chan] = 0;
            m_y[i][chan] = 0;
        }
    }
}

void BiquadFilterProcessor::setParam(int paramId, float value)
{
    switch (paramId)
    {
        case kCkBiquadFilterParam_FilterType:
            m_type = Math::clamp((int) value, 0, (int) kCkBiquadFilterParam_FilterType_HighShelf);
            m_calc = true;
            break;

        case kCkBiquadFilterParam_Freq:
            m_freq = Math::max(value, 0.01f);
            m_calc = true;
            break;

        case kCkBiquadFilterParam_Q:
            m_q = Math::max(value, 0.01f);
            m_calc = true;
            break;

        case kCkBiquadFilterParam_Gain:
            m_gain = value;
            m_calc = true;
            break;

        default:
            CK_LOG_ERROR("Biquad Filter effect: unknown param ID %d", paramId);
            break;
    }
}

void BiquadFilterProcessor::process_default(int* inBuf, int* outBuf, int frames)
{
    if (m_calc)
    {
        calcCoeffs(true);
        m_calc = false;
    }

    // history
    int64 x00 = m_x[0][0].fixedValue;
    int64 x01 = m_x[0][1].fixedValue;
    int64 x10 = m_x[1][0].fixedValue;
    int64 x11 = m_x[1][1].fixedValue;
    int64 y00 = m_y[0][0].fixedValue;
    int64 y01 = m_y[0][1].fixedValue;
    int64 y10 = m_y[1][0].fixedValue;
    int64 y11 = m_y[1][1].fixedValue;


    // copy coeffs
    int64 b0a0 = m_b0a0.fixedValue;
    int64 b1a0 = m_b1a0.fixedValue;
    int64 b2a0 = m_b2a0.fixedValue;
    int64 a1a0 = m_a1a0.fixedValue;
    int64 a2a0 = m_a2a0.fixedValue;

    int64 x0;
    int64 y0;

    int32* p = inBuf;
    int32* pEnd = p + frames*2;
    while (p < pEnd)
    {
        // left

        x0 = *p;

        y0 = (b0a0 * x0 + 
              b1a0 * x00 + 
              b2a0 * x10 - 
              a1a0 * y00 - 
              a2a0 * y10) >> 24;
        
        *p++ = (int32) y0;

        x10 = x00;
        x00 = x0;
        y10 = y00;
        y00 = y0;


        // right

        x0 = *p;

        y0 = (b0a0 * x0 + 
              b1a0 * x01 + 
              b2a0 * x11 - 
              a1a0 * y01 - 
              a2a0 * y11) >> 24;
        
        *p++ = (int32) y0;

        x11 = x01;
        x01 = x0;
        y11 = y01;
        y01 = y0;
    }

    m_x[0][0] = (int32) x00;
    m_x[0][1] = (int32) x01;
    m_x[1][0] = (int32) x10;
    m_x[1][1] = (int32) x11;
    m_y[0][0] = (int32) y00;
    m_y[0][1] = (int32) y01;
    m_y[1][0] = (int32) y10;
    m_y[1][1] = (int32) y11;
}

void BiquadFilterProcessor::process_default(float* inBuf, float* outBuf, int frames)
{
    if (m_calc)
    {
        calcCoeffs(false);
        m_calc = false;
    }

    // history
    float x00 = m_x[0][0].floatValue;
    float x01 = m_x[0][1].floatValue;
    float x10 = m_x[1][0].floatValue;
    float x11 = m_x[1][1].floatValue;
    float y00 = m_y[0][0].floatValue;
    float y01 = m_y[0][1].floatValue;
    float y10 = m_y[1][0].floatValue;
    float y11 = m_y[1][1].floatValue;


    // copy coeffs
    float b0a0 = m_b0a0.floatValue;
    float b1a0 = m_b1a0.floatValue;
    float b2a0 = m_b2a0.floatValue;
    float a1a0 = m_a1a0.floatValue;
    float a2a0 = m_a2a0.floatValue;

    float x0;
    float y0;

    float* p = inBuf;
    float* pEnd = p + frames*2;
    while (p < pEnd)
    {
        // left

        x0 = *p;

        y0 = b0a0 * x0 + 
             b1a0 * x00 + 
             b2a0 * x10 - 
             a1a0 * y00 - 
             a2a0 * y10;
        
        *p++ = y0;

        x10 = x00;
        x00 = x0;
        y10 = y00;
        y00 = y0;


        // right

        x0 = *p;

        y0 = b0a0 * x0 + 
             b1a0 * x01 + 
             b2a0 * x11 - 
             a1a0 * y01 - 
             a2a0 * y11;
        
        *p++ = y0;

        x11 = x01;
        x01 = x0;
        y11 = y01;
        y01 = y0;
    }

    m_x[0][0] = x00;
    m_x[0][1] = x01;
    m_x[1][0] = x10;
    m_x[1][1] = x11;
    m_y[0][0] = y00;
    m_y[0][1] = y01;
    m_y[1][0] = y10;
    m_y[1][1] = y11;
}

////////////////////////////////////////

void BiquadFilterProcessor::calcCoeffs(bool fixedPoint)
{
    float a0 = 0.0f;
    float a1 = 0.0f;
    float a2 = 0.0f;
    float b0 = 0.0f;
    float b1 = 0.0f;
    float b2 = 0.0f;

    float w0 = Math::k_2pi * m_freq / getSampleRate();
    float cosW0 = Math::cos(w0);
    float sinW0 = Math::sin(w0);

    float alpha = sinW0 / (2.0f * m_q);
    float A = Math::pow(10.0f, m_gain/40.0f);

    switch (m_type)
    {
        case kCkBiquadFilterParam_FilterType_LowPass:
            {
                b0 = (1.0f - cosW0) * 0.5f;
                b1 = (1.0f - cosW0);
                b2 = b0;
                a0 = 1.0f + alpha;
                a1 = -2.0f * cosW0;
                a2 = 1.0f - alpha;
            }
            break;

        case kCkBiquadFilterParam_FilterType_HighPass:
            {
                b0 = (1.0f + cosW0) * 0.5f;
                b1 = -(1.0f + cosW0);
                b2 = b0;
                a0 = 1.0f + alpha;
                a1 = -2.0f * cosW0;
                a2 = 1.0f - alpha;
            }
            break;

        case kCkBiquadFilterParam_FilterType_BandPass:
            {
                b0 = alpha;
                b1 = 0.0f;
                b2 = -alpha;
                a0 = 1.0f + alpha;
                a1 = -2.0f * cosW0;
                a2 = 1.0f - alpha;
            }
            break;

        case kCkBiquadFilterParam_FilterType_Notch:
            {
                b0 = 1.0f;
                b1 = -2.0f * cosW0;
                b2 = 1.0f;
                a0 = 1.0f + alpha;
                a1 = -2.0f * cosW0;
                a2 = 1.0f - alpha;
            }
            break;

        case kCkBiquadFilterParam_FilterType_Peak:
            {
                b0 = 1.0f + alpha*A;
                b1 = -2.0f * cosW0;
                b2 = 1.0f - alpha*A;
                a0 = 1.0f + alpha/A;
                a1 = -2.0f * cosW0;
                a2 = 1.0f - alpha/A;
            }
            break;

        case kCkBiquadFilterParam_FilterType_LowShelf:
            {
                float x = 2.0f * Math::sqrt(A) * alpha;
                b0 =        A * ((A+1.0f) - (A-1.0f)*cosW0 + x);
                b1 = 2.0f * A * ((A-1.0f) - (A+1.0f)*cosW0);
                b2 =        A * ((A+1.0f) - (A-1.0f)*cosW0 - x);
                a0 =             (A+1.0f) + (A-1.0f)*cosW0 + x;
                a1 =    -2.0f * ((A-1.0f) + (A+1.0f)*cosW0);
                a2 =             (A+1.0f) + (A-1.0f)*cosW0 - x;
            }
            break;

        case kCkBiquadFilterParam_FilterType_HighShelf:
            {
                float x = 2.0f * Math::sqrt(A) * alpha;
                b0 =         A * ((A+1.0f) + (A-1.0f)*cosW0 + x);
                b1 = -2.0f * A * ((A-1.0f) + (A+1.0f)*cosW0);
                b2 =         A * ((A+1.0f) + (A-1.0f)*cosW0 - x);
                a0 =              (A+1.0f) - (A-1.0f)*cosW0 + x;
                a1 =      2.0f * ((A-1.0f) - (A+1.0f)*cosW0);
                a2 =              (A+1.0f) - (A-1.0f)*cosW0 - x;
            }
            break;

        default:
            CK_LOG_ERROR("Unknown biquad filter type: %d", m_type);
            break;
    }

    if (fixedPoint)
    {
        m_b0a0 = Fixed8_24::fromFloat(b0 / a0);
        m_b1a0 = Fixed8_24::fromFloat(b1 / a0);
        m_b2a0 = Fixed8_24::fromFloat(b2 / a0);
        m_a1a0 = Fixed8_24::fromFloat(a1 / a0);
        m_a2a0 = Fixed8_24::fromFloat(a2 / a0);
    }
    else
    {
        m_b0a0 = b0 / a0;
        m_b1a0 = b1 / a0;
        m_b2a0 = b2 / a0;
        m_a1a0 = a1 / a0;
        m_a2a0 = a2 / a0;
    }
}

}
