#pragma once

#include "ck/core/platform.h"
#include "ck/audio/streamsound.h"
#include "ck/audio/streamsource.h"
#include "ck/audio/vorbisaudiostream.h"


namespace Cki
{


class VorbisStreamSound : public StreamSound
{
public:
    VorbisStreamSound(const char* path, CkPathType, int offset, int length);

private:
    VorbisAudioStream m_stream;
    StreamSource m_source;
};



}



