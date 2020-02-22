#pragma once

#include "ck/core/platform.h"
#include "ck/audio/streamsound.h"
#include "ck/audio/streamsource.h"
#include "ck/audio/cksaudiostream.h"

namespace Cki
{


class CksStreamSound : public StreamSound
{
public:
    CksStreamSound(const char* path, CkPathType, int offset, int length);

private:
    CksAudioStream m_stream;
    StreamSource m_source;
};



}

