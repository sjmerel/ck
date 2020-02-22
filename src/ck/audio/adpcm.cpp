#include "ck/audio/adpcm.h"
#include "ck/core/mem.h"
#include "ck/core/debug.h"
#include "ck/core/math.h"

namespace Cki
{

namespace
{
    // follows http://netghost.narod.ru/gff/vendspec/micriff/ms_riff.txt
    // ("Microsoft Multimedia Standards Update" from 1992)

    // These are effectively 8.8 fixed point values, used for predicting the next sample:
    //   predSamp = ( (samp1 * coef1) + (samp2 * coef2) ) / 256
    // where samp1 is the previous sample and samp2 is the sample before that.

    int g_coeffs[][2] =
    {
        // Standard Microsoft coefficients:

        { 256,  0    }, // 1.0,  0.0    // constant, same as last sample
        { 512,  -256 }, // 2.0,  -1.0   // linear extrapolation
        { 0,    0    }, // 0.0,  0.0    // always predicts 0, for silent sections
        { 192,  64   }, // 0.75, 0.25
        { 240,  0    }, // 0.94, 0
        { 460,  -208 }, // 1.80, -0.81
        { 392,  -232 }, // 1.53, -0.91

        /*
        // Some other ones... they do decrease error for some sounds, but not perceptibly
        { 0,    256  },
        { -256, 512  },
        { 64,   192  },
        { 0,    240  },
        { -208, 460  },
        { -232, 392  },
        */
    };
    const int k_numCoeffs = CK_ARRAY_COUNT(g_coeffs);

    const int g_adaptionTable[] = 
    {
        230, 230, 230, 230, 307, 409, 512, 614,
        768, 614, 512, 409, 307, 230, 230, 230
    };

    const int k_fixedPointCoefBase = 256;
    const int k_fixedPointAdaptionBase = 256;
    const int k_minDelta = 16;
}

namespace Adpcm
{
    int getBlockSize(int numSamples)
    {
        return 6 + numSamples / 2;
    }

    int getNumPredictors()
    {
        return k_numCoeffs;
    }

    int encode(const int16* input, int inputSamples, int inputStride, byte* output, int predictor, int& delta_)
    {
        CK_ASSERT(inputSamples % 2 == 0);
        CK_ASSERT(inputStride == 1 || inputStride == 2);

        byte* pOut = output;
        const int16* inputEnd = input + inputSamples*inputStride;

        *pOut++ = (uint8) predictor;
        CK_ASSERT(predictor <= k_numCoeffs);

        int16 delta = (int16) delta_;
        CK_ASSERT(delta >= k_minDelta);
        Mem::copy(pOut, &delta, sizeof(delta));
        pOut += sizeof(delta);

        int16 samp2 = *input;
        input += inputStride;
        Mem::copy(pOut, &samp2, sizeof(samp2));
        pOut += sizeof(samp2);

        int16 samp1 = *input;
        input += inputStride;
        Mem::copy(pOut, &samp1, sizeof(samp1));
        pOut += sizeof(samp1);

        int coef1 = g_coeffs[predictor][0];
        int coef2 = g_coeffs[predictor][1];

        while (input < inputEnd)
        {
            uint8 outByte = 0;
            for (int nybble = 0; nybble < 2; ++nybble)
            {
                int predSamp = ((samp1 * coef1) + (samp2 * coef2)) / k_fixedPointCoefBase;

                int errorDelta = Math::clamp((*input - predSamp) / delta, -8, 7);

                outByte |= ((errorDelta & 0xf) << (nybble*4));
                int newSamp = Math::clamp(predSamp + (delta * errorDelta), CK_INT16_MIN, CK_INT16_MAX);

                delta = (int16) (delta * g_adaptionTable[errorDelta & 0xf] / k_fixedPointAdaptionBase);
                if (delta < k_minDelta) 
                {
                    delta = k_minDelta;
                }

                samp2 = samp1;
                samp1 = (int16) newSamp;
                input += inputStride;
            }
            *pOut++ = outByte;
        }

        delta_ = delta;

        int outputBytes = (int)(pOut - output);
        CK_ASSERT(outputBytes == inputSamples/2 + 6);
        return outputBytes;
    }

    int decode(const byte* input, int inputBytes, int16* output, int outputStride)
    {
        CK_ASSERT(outputStride == 1 || outputStride == 2);
        CK_ASSERT(inputBytes >= 7);

        const byte* inputEnd = input + inputBytes;
        int16* outputStart = output;

        uint8 predictor = *input++;
        CK_ASSERT(predictor <= k_numCoeffs);

        int16 delta;
        Mem::copy(&delta, input, sizeof(delta));
        CK_ASSERT(delta >= k_minDelta);
        input += sizeof(delta);

        int16 samp2;
        Mem::copy(&samp2, input, sizeof(samp2));
        input += sizeof(samp2);

        int16 samp1;
        Mem::copy(&samp1, input, sizeof(samp1));
        input += sizeof(samp1);

        *output = samp2;
        output += outputStride;
        *output = samp1;
        output += outputStride;

        int coef1 = g_coeffs[predictor][0];
        int coef2 = g_coeffs[predictor][1];

        while (input < inputEnd)
        {
            for (int nybble = 0; nybble < 2; ++nybble)
            {
                int predSamp = ((samp1 * coef1) + (samp2 * coef2)) / k_fixedPointCoefBase;
                int errorDelta = (*input >> (nybble*4)) & 0xf;
                if (errorDelta & 0x8) // negative
                {
                    predSamp += delta * (errorDelta - 0x10);
                }
                else
                {
                    predSamp += delta * errorDelta;
                }
                int newSamp = Math::clamp(predSamp, CK_INT16_MIN, CK_INT16_MAX);
                *output = (int16) newSamp;
                output += outputStride;

                delta = (int16) (delta * g_adaptionTable[errorDelta] / k_fixedPointAdaptionBase);
                if (delta < k_minDelta) 
                {
                    delta = k_minDelta;
                }

                samp2 = samp1;
                samp1 = (int16) newSamp;
            }
            ++input;
        }

        int outputSamples = (int)(output - outputStart) / outputStride;
        CK_ASSERT(outputSamples == 2*inputBytes - 12);
        return outputSamples;
    }

}

}
