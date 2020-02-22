#pragma once

#include "ck/core/platform.h"
#include "ck/core/types.h"


namespace Cki
{


namespace Adpcm
{
    // TODO block size should be configurable
    enum { k_samplesPerBlock = 36 }; 
    enum { k_bytesPerBlock = 24 }; 

    int getBlockSize(int numSamples);
    int getNumPredictors();

    // Returns number of bytes encoded (which will be equal to 6 + inputSamples/2)
    int encode(const int16* input, int inputSamples, int inputStride, byte* output, int predictor, int& delta);

    // Returns number of samples decoded (which will be equal to 2*inputBytes - 12)
    int decode(const byte* input, int inputBytes, int16* output, int outputStride);
}


}
