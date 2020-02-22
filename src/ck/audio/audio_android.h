#pragma once

#include "ck/core/platform.h"

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

namespace Cki
{

namespace Audio
{
    extern SLObjectItf g_engineObj;
    extern SLEngineItf g_engine;
    extern SLObjectItf g_outputMixObj;

    int getNativeSampleRate();
}


}

