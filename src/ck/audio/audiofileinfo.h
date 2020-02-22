#pragma once

#include "ck/core/platform.h"
#include "ck/core/readstream.h"

namespace Cki
{


class AudioFileInfo
{
public:
    enum Format
    {
        k_unknown = -1,

        k_wav,
        k_mp3,
        k_oggVorbis,

        k_numFormats
    };

    AudioFileInfo(const char* path, CkPathType);

    bool isValid() const;
    Format getFormat() const;
    int getSampleRate() const;
    int getChannels() const;

private:
    ReadStream m_stream;
    Format m_format;
    int m_sampleRate;
    int m_channels;

    void readAll(Format expectedType);
    bool read(Format type);

    ////////////////////////////////////////

    bool readWav();
    bool readWavChunk();

    ////////////////////////////////////////

    bool readMp3();
    bool readMp3Tag();
    bool readMp3TagId3v1();
    bool readMp3TagId3v2();
    bool readMp3TagApev2();
    bool readMp3Frame();

    ////////////////////////////////////////

    bool readOggVorbis();
    bool readOggVorbisPacket(int segLength);
};


}
