#pragma once

#include "ck/core/platform.h"
#include "ck/audio/streamsound.h"
#include "ck/audio/streamsource.h"
#include "ck/audio/customaudiostream.h"

namespace Cki
{


class CustomStreamSound : public StreamSound
{
public:
    CustomStreamSound(CkCustomStream*);

private:
    CustomAudioStream m_stream;
    StreamSource m_source;
};



}


