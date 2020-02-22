#include "ck/audio/effectprocessor.h"
#include "ck/core/platform.h"
#include "ck/core/system.h"

namespace Cki
{


void EffectProcessor::process(int* inBuf, int* outBuf, int frames)
{
    processImpl(inBuf, outBuf, frames);
}

void EffectProcessor::process(float* inBuf, float* outBuf, int frames)
{
    processImpl(inBuf, outBuf, frames);
}

template <typename T>
void EffectProcessor::processImpl(T* inBuf, T* outBuf, int frames)
{
    if (System::get()->isSimdSupported())
    {
#if CK_ARCH_ARM
        process_neon(inBuf, outBuf, frames);
#else // CK_ARCH_X86
        process_sse(inBuf, outBuf, frames);
#endif
    }
    else
    {
        process_default(inBuf, outBuf, frames);
    }
}


}
