#pragma once

#include "ck/core/platform.h"
#include "ck/audio/streamsound.h"
#include "ck/audio/streamsource.h"
#include "ck/audio/assetaudiostream_ios.h"


namespace Cki
{


class AssetStreamSound : public StreamSound
{
public:
    AssetStreamSound(const char* url);

private:
    AssetAudioStream m_stream;
    StreamSource m_source;
};



}



