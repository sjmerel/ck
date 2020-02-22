#pragma once

#include "ck/core/platform.h"
#include "ck/audio/streamsound.h"
#include "ck/audio/streamsource.h"
#include "ck/audio/nativeaudiostream_android.h"


namespace Cki
{


class NativeStreamSoundIcs : public StreamSound
{
public:
    NativeStreamSoundIcs(const char* uri, bool isAsset);

private:
    NativeAudioStream m_stream;
    StreamSource m_source;
};



}



