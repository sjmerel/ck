#include "ck/audio/audiofileinfo.h"
#include "ck/core/string.h"
#include "ck/core/debug.h"
#include "ck/audio/fourcharcode.h"

namespace Cki
{

namespace
{
    // guess file type from file extension
    AudioFileInfo::Format guessFileType(const char* path)
    {
        String pathStr((char*) path, String::External());
        if (pathStr.endsWith(".wav", true))
        {
            return AudioFileInfo::k_wav;
        }
        if (pathStr.endsWith(".mp3", true))
        {
            return AudioFileInfo::k_mp3;
        }
        if (pathStr.endsWith(".ogg", true) || pathStr.endsWith(".oga", true))
        {
            return AudioFileInfo::k_oggVorbis;
        }

        return AudioFileInfo::k_unknown;
    }
}


AudioFileInfo::AudioFileInfo(const char* path, CkPathType pathType) :
    m_stream(path, pathType),
    m_format(k_unknown),
    m_sampleRate(-1),
    m_channels(-1)
{
    // guess expected format
    if (m_stream.isValid())
    {
        Format expectedType = guessFileType(path);
        readAll(expectedType);
    }
}

bool AudioFileInfo::isValid() const
{
    return m_format != k_unknown;
}

AudioFileInfo::Format AudioFileInfo::getFormat() const
{
    return m_format;
}

int AudioFileInfo::getSampleRate() const
{
    return m_sampleRate;
}

int AudioFileInfo::getChannels() const
{
    return m_channels;
}

////////////////////////////////////////

void AudioFileInfo::readAll(Format expectedType)
{
    if (expectedType != k_unknown)
    {
        if (read(expectedType))
        {
            return;
        }
    }

    for (int i = 0; i < k_numFormats; ++i)
    {
        Format type = (Format) i;
        if (expectedType == type)
        {
            continue; // already tried that
        }
        if (read(type))
        {
            return;
        }
    }
}

bool AudioFileInfo::read(Format type)
{
    m_stream.setPos(0);

    bool success = false;
    switch (type)
    {
        case k_wav:
            success = readWav();
            break;

        case k_mp3:
            success = readMp3();
            break;

        case k_oggVorbis:
            success = readOggVorbis();
            break;

        default:
            break;
    }

    if (success)
    {
        m_format = type;
    }
    return success;
}

////////////////////////////////////////

bool AudioFileInfo::readWav()
{
    if (m_stream.getSize() < 12)
    {
        return false;
    }

    FourCharCode chunkId;
    m_stream >> chunkId;
    if (chunkId != FourCharCode('R', 'I', 'F', 'F'))
    {
        return false;
    }

    uint32 chunkSize;
    m_stream >> chunkSize;
    if (chunkSize > (uint32) m_stream.getSize() - 8)
    {
        return false;
    }

    FourCharCode formatId;
    m_stream >> formatId;
    if (formatId != FourCharCode('W', 'A', 'V', 'E'))
    {
        return false;
    }

    // parse chunks
    int endPos = chunkSize + 8;
    while (m_stream.getPos() < endPos)
    {
        if (readWavChunk())
        {
            return true;
        }
    }

    return false;
}

// returns true if read format chunk
bool AudioFileInfo::readWavChunk()
{
    FourCharCode chunkId;
    m_stream >> chunkId;

    uint32 chunkSize;
    m_stream >> chunkSize;

    int startPos = m_stream.getPos();
    int endPos = startPos + chunkSize;
    if (endPos & 1)
    {
        endPos += 1; // pad byte
    }

    if (chunkId == FourCharCode('f', 'm', 't', ' '))
    {
        m_stream.skip(2); // format

        uint16 channels;
        m_stream >> channels;
        m_channels = channels;

        uint32 samplesPerSec;
        m_stream >> samplesPerSec;
        m_sampleRate = samplesPerSec;

        return true;
    }
    else
    {
        m_stream.skip(chunkSize);
        return false;
    }
}

////////////////////////////////////////

bool AudioFileInfo::readMp3()
{
    while (readMp3Tag())
        ;
    return readMp3Frame();
}

bool AudioFileInfo::readMp3Tag()
{
    // TODO APEv2 tag
    return readMp3TagId3v1() || readMp3TagId3v2() || readMp3TagApev2();
}

bool AudioFileInfo::readMp3TagId3v1()
{
    int pos = m_stream.getPos();

    char tag[4] = {0};
    m_stream.read(&tag, sizeof(tag));
    if (tag[0] == 'T' && tag[1] == 'A' && tag[2] == 'G')
    {
        // ID3v1

        if (tag[3] == '+')
        {
            // extended
            m_stream.skip(227-sizeof(tag));
        }
        else
        {
            m_stream.skip(128-sizeof(tag));
        }

        return true;
    }

    m_stream.setPos(pos);
    return false;
}

bool AudioFileInfo::readMp3TagId3v2()
{
    int pos = m_stream.getPos();

    byte tag[10] = {0};
    m_stream.read(&tag, sizeof(tag));

    if (tag[0] == 'I' && tag[1] == 'D' && tag[2] == '3' &&  // id
            tag[3] < 0xff && tag[4] < 0xff && // version
            tag[6] < 0x80 && tag[7] < 0x80 && tag[8] < 0x80 && tag[9] < 0x80)// size
    {
        // ID3v2
        int size = tag[9] | (tag[8] << 7) | (tag[7] << 14) | (tag[6] << 21);
        m_stream.skip(size);
        return true;
    }

    m_stream.setPos(pos);
    return false;
}

bool AudioFileInfo::readMp3TagApev2()
{
    int pos = m_stream.getPos();

    char tag[8] = {0};
    m_stream.read(&tag, sizeof(tag));

    if (String::equals(tag, "APETAGEX", 8))
    {
        m_stream.skip(4);
        uint32 size;
        m_stream >> size;
        m_stream.skip(size - 16);
        return true;
    }

    m_stream.setPos(pos);
    return false;
}

bool AudioFileInfo::readMp3Frame()
{
    // see http://www.mpgedit.org/mpgedit/mpeg_format/MP3Format.html
    // 
    // bits are labeled: 
    //  AAAAAAAA AAABBCCD EEEEFFGH IIJJKLMM

    byte header[4] = { 0 };
    m_stream.read(&header, sizeof(header));

    // bits A: frame sync
    if (header[0] != 0xff || (header[1] & 0xe0) != 0xe0)
    {
        return false; // bad frame sync
    }

    // bits E: bitrate index
    if ((header[2] & 0xf0) == 0xf0)
    {
        return false; // bad bitrate index
    }

    int versionId = ((header[1] & 0x18) >> 3); // bits B
    int sampleRateIndex = ((header[2] & 0xc) >> 2); // bits F
    switch (versionId)
    {
        case 0: // MPEG version 2.5
            switch (sampleRateIndex)
            {
                case 0: m_sampleRate = 11025; break;
                case 1: m_sampleRate = 12000; break;
                case 2: m_sampleRate = 8000; break;
                case 3: return false;
            }
            break;

        case 1: // reserved; assume bad file
            return false;

        case 2: // MPEG version 2
            switch (sampleRateIndex)
            {
                case 0: m_sampleRate = 22050; break;
                case 1: m_sampleRate = 24000; break;
                case 2: m_sampleRate = 16000; break;
                case 3: return false;
            }
            break;

        case 3: // MPEG version 1
            switch (sampleRateIndex)
            {
                case 0: m_sampleRate = 44100; break;
                case 1: m_sampleRate = 48000; break;
                case 2: m_sampleRate = 32000; break;
                case 3: return false;
            }
            break;
    }

    // bits I: channel mode
    if ((header[3] & 0xc0) == 0xc0)
    {
        m_channels = 1;
    }
    else
    {
        m_channels = 2;
    }

    return true;
}

////////////////////////////////////////

bool AudioFileInfo::readOggVorbis()
{
    FourCharCode chunkId;
    m_stream >> chunkId;
    if (chunkId != FourCharCode('O', 'g', 'g', 'S'))
    {
        return false;
    }

    m_stream.skip(22);

    uint8 numSegments = 0;
    m_stream >> numSegments;

    // identification header is always the first packet
    m_stream.skip(numSegments);

    uint8 packetType = 0;
    m_stream >> packetType;
    if (packetType != 1)
    {
        return false;
    }

    char vorbis[6] = { 0 };
    m_stream.read(vorbis, sizeof(vorbis));
    if (!String::equals(vorbis, "vorbis", sizeof(vorbis)))
    {
        return false;
    }

    m_stream.skip(4); // version

    uint8 channels;
    m_stream >> channels;
    m_channels = channels;

    uint32 sampleRate;
    m_stream >> sampleRate;
    m_sampleRate = sampleRate;

    return true;
}


}
