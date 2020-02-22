#include "ck/effectprocessor.h"
#include "ck/audio/audionode.h"
#include "ck/core/mem.h"


int CkEffectProcessor::getSampleRate()
{
    return Cki::AudioNode::getSampleRate();
}

void CkEffectProcessor::process(int* inBuf, int* outBuf, int frames)
{
    if (!isInPlace())
    {
        Cki::Mem::copy(outBuf, inBuf, frames * 2 * sizeof(int));
    }
}

