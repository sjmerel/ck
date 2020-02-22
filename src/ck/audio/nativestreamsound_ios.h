#pragma once

#include "ck/core/platform.h"
#include "ck/audio/streamsound.h"
#include "ck/audio/streamsource.h"
#include "ck/audio/nativeaudiostream_ios.h"


namespace Cki
{


class NativeStreamSound : public StreamSound
{
public:
    NativeStreamSound(const char* path, int offset, int length);

private:
    NativeAudioStream m_stream;
    StreamSource m_source;
};



}


