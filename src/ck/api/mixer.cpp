#include "ck/mixer.h"
#include "ck/audio/mixer.h"


CkMixer* CkMixer::getMaster()
{
    return (CkMixer*) Cki::Mixer::getMaster();
}

CkMixer* CkMixer::newMixer(const char* name, CkMixer* parent)
{
    return (CkMixer*) Cki::Mixer::newMixer(name, (Cki::Mixer*) parent);
}

CkMixer* CkMixer::find(const char* name)
{
    return (CkMixer*) Cki::Mixer::find(name);
}
