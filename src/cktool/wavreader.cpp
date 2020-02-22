#include "wavreader.h"
#include "guid.h"
#include "ck/core/debug.h"
#include "ck/core/logger.h"
#include "ck/core/math.h"
#include "ck/audio/fourcharcode.h"

// see: 
// http://www-mmsp.ece.mcgill.ca/documents/audioformats/wave/wave.html
// http://www-mmsp.ece.mcgill.ca/documents/audioformats/wave/Docs/riffmci.pdf
// http://www-mmsp.ece.mcgill.ca/documents/audioformats/wave/Docs/RIFFNEW.pdf

namespace Cki
{

namespace
{

    // RIFF format marker
    const FourCharCode k_riff('R', 'I', 'F', 'F');

    // WAVE file type marker
    const FourCharCode k_wave('W', 'A', 'V', 'E');

    // chunk types
    const FourCharCode k_fmt ('f', 'm', 't', ' ');
    const FourCharCode k_data('d', 'a', 't', 'a');
    const FourCharCode k_cue ('c', 'u', 'e', ' ');
    const FourCharCode k_smpl('s', 'm', 'p', 'l');


    // list chunk type
    const FourCharCode k_LIST('L', 'I', 'S', 'T');

    // chunk list types
    const FourCharCode k_adtl('a', 'd', 't', 'l');
    const FourCharCode k_INFO('I', 'N', 'F', 'O');

    // list subchunk types
    //  adtl:
    const FourCharCode k_labl('l', 'a', 'b', 'l');
    const FourCharCode k_note('n', 'o', 't', 'e');
    const FourCharCode k_ltxt('l', 't', 'x', 't');


    const Guid k_pcmGuid(0x1, 0x0, 0x10, 0x800000aa00389b71);
}



WavReader::WavReader(const char* filename) :
    m_file(filename, FileStream::k_read),
    m_swapper(m_file, ByteOrder::k_littleEndian),
    m_valid(false),
    m_dataSize(0),
    m_readPos(0),
    m_curChunk(0),
    m_channels(0),
    m_sampleRate(0),
    m_blockAlign(0),
    m_bitsPerSample(0),
    m_float(false)
{
    if (!m_file.isValid() || m_file.getSize() < 12)
    {
        return;
    }

    // RIFF chunk
    FourCharCode chunkId;
    m_swapper >> chunkId;
    if (chunkId != k_riff)
    {
        return;
    }

    uint32 chunkSize;
    m_swapper >> chunkSize;
    // RIFF chunk size should be 8 bytes less than file size; however I found
    // at least one file which said 4 bytes less (probably mistunderstood the spec).
    // Allow this; each sub-chunk has its own size anyway.
    if (chunkSize > (uint32) m_file.getSize() - 4)
    {
        return;
    }

    FourCharCode formatId;
    m_swapper >> formatId;
    if (formatId != k_wave)
    {
        return;
    }

    // parse chunks
    int endPos = chunkSize + 8;
    while (m_file.getPos() < endPos)
    {
        if (!parseChunk())
        {
            return;
        }
    }

    // m_valid is set to true if we read a format chunk

    // we need data, too
    m_valid &= !m_dataChunkInfo.empty();

    // data size must be a multiple of block size
    m_valid &= (m_dataSize % m_blockAlign == 0);

    if (m_valid)
    {
        // seek to beginning of data
        m_file.setPos(m_dataChunkInfo[0].m_offset);

        if (m_float)
        {
            // calculate normalization factor
            int pos = m_file.getPos();

            int samples = m_dataSize / sizeof(float);
            for (int i = 0; i < samples; ++i)
            {
                float tmp;
                if (readImpl((byte*) &tmp, sizeof(float)) == sizeof(float))
                {
                    if (ByteOrder::k_native != ByteOrder::k_littleEndian)
                    {
                        swapSample((byte*) &tmp, m_bitsPerSample);
                    }
                    m_floatNorm = Math::max(m_floatNorm, fabsf(tmp));
                }
                else
                {
                    break;
                }
            }

            if (m_floatNorm > 1.0f)
            {
                CK_LOG_WARNING("Input file's maximum amplitude is %f; will be renormalized to 1.", m_floatNorm);
            }

            m_file.setPos(pos);
            m_curChunk = 0;
            m_readPos = 0;
        }
    }
}

bool WavReader::isValid() const
{
    return m_valid;
}

int WavReader::getBytes() const
{
    return m_dataSize;
}

int WavReader::getFrames() const
{
    return m_dataSize / (m_channels * (m_bitsPerSample / 8));
}

int WavReader::getChannels() const
{
    return m_channels;
}

int WavReader::getBitsPerSample() const
{
    return m_bitsPerSample;
}

int WavReader::getSampleRate() const
{
    return m_sampleRate;
}

int WavReader::getNumMarkers() const
{
    return (int) m_markers.size();
}

void WavReader::getMarkerInfo(int index, const char** name, int& start, int& end) const
{
    const Marker& marker = m_markers[index];

    if (name)
    {
        *name = getMarkerName(marker.m_id);
    }

    start = marker.m_start;

    end = start + getMarkerLength(marker.m_id);
}

int WavReader::getNumLoops() const
{
    return (int) m_loops.size();
}

void WavReader::getLoopInfo(int index, const char** name, int& start, int& end, int& count) const
{
    const Loop& loop = m_loops[index];

    if (name)
    {
        *name = getMarkerName(loop.m_id);
    }

    start = loop.m_start;
    end = loop.m_end;
    count = loop.m_count;
}

int WavReader::read(int16* buf, int samples)
{
    int bytesPerBlock = m_blockAlign / m_channels;
    if (m_bitsPerSample == 16 && bytesPerBlock == 2 && ByteOrder::k_native == ByteOrder::k_littleEndian)
    {
        // stored as 16-bit little-endian PCM, just copy it out
        int bytesRead = readImpl(buf, samples * sizeof(int16));
        return bytesRead / sizeof(int16);
    }
    else
    {
        byte tmp[4]; // holds up to 32 bits
        CK_ASSERT(bytesPerBlock <= 4);

        for (int i = 0; i < samples; ++i)
        {
            int bytesRead = readImpl(tmp, bytesPerBlock);
            if (bytesRead == bytesPerBlock)
            {
                // TODO what if bytes per sample < bytes per block?
                if (ByteOrder::k_native != ByteOrder::k_littleEndian)
                {
                    swapSample(tmp, m_bitsPerSample);
                }

                if (m_float)
                {
                    *buf++ = convertFloatSample(tmp);
                }
                else
                {
                    if (m_bitsPerSample == 8)
                    {
                        // 8-bit wavs are unsigned; convert
                        tmp[0] = (byte) (((int16) tmp[0]) - 128);
                    }
                    *buf++ = convertIntSample(tmp, m_bitsPerSample);
                }
            }
            else
            {
                return i;
            }
        }
        return samples;
    }
}

int WavReader::readImpl(void* buf, int bytes)
{
    if (!m_valid)
    {
        return 0;
    }

    char* p = (char*) buf;
    int bytesLeftInFile = m_dataSize - m_readPos;
    int bytesToRead = Math::min(bytes, bytesLeftInFile);
    int bytesRead = 0;
    while (bytesRead < bytesToRead)
    {
        const DataChunkInfo dci = m_dataChunkInfo[m_curChunk];
        int bytesLeftInChunk = dci.m_size - (m_file.getPos() - dci.m_offset);
        int bytesToCopy = Math::min(bytesLeftInChunk, bytesToRead);

        m_file.read(p, bytesToCopy);
        p += bytesToCopy;
        bytesRead += bytesToCopy;

        if (bytesToCopy == bytesLeftInChunk)
        {
            // hit end of chunk
            CK_ASSERT(m_file.getPos() == dci.m_offset + dci.m_size);
            ++m_curChunk;
        }
    }
    CK_ASSERT(bytesRead <= bytesToRead);
    m_readPos += bytesRead;

    return bytesRead;
}

bool WavReader::isAtEnd() const
{
    return m_readPos >= m_dataSize;
}

////////////////////////////////////////

const char* WavReader::getMarkerName(uint32 id) const
{
    std::map<uint32, String>::const_iterator it = m_markerNames.find(id);
    if (it == m_markerNames.end())
    {
        return NULL;
    }
    else
    {
        return it->second.getBuffer();
    }
}

int WavReader::getMarkerLength(uint32 id) const
{
    std::map<uint32, int>::const_iterator it = m_markerLengths.find(id);
    if (it == m_markerLengths.end())
    {
        return 0;
    }
    else
    {
        return it->second;
    }
}

bool WavReader::parseChunk()
{
    FourCharCode chunkId;
    m_swapper >> chunkId;

    uint32 chunkSize;
    m_swapper >> chunkSize;

    int endPos = m_file.getPos() + chunkSize;
    if (endPos & 1)
    {
        endPos += 1; // pad byte
    }

    bool success = true;
    if (chunkId == k_LIST)
    {
        // special list chunks
        success = parseChunkList(chunkSize);
    }
    else if (chunkId == k_fmt)
    {
        success = parseFmtChunk(chunkSize);
    }
    else if (chunkId == k_data)
    {
        success = parseDataChunk(chunkSize);
    }
    else if (chunkId == k_cue)
    {
        success = parseCueChunk(chunkSize);
    }
    else if (chunkId == k_smpl)
    {
        success = parseSmplChunk(chunkSize);
    }
    else
    {
        CK_LOG_INFO("---  skipping chunk '%.4s' at offset %d (%d bytes)", chunkId.getData(), m_file.getPos(), chunkSize);
        m_file.skip(chunkSize);
    }

    // skip unread bytes
    int pos = m_file.getPos();
    CK_ASSERT(pos <= endPos);
    if (pos < endPos)
    {
        m_file.skip(endPos - pos);
    }

    return success;
}

bool WavReader::parseFmtChunk(uint32 chunkSize)
{
    uint16 formatTag;
    m_swapper >> formatTag;

    uint16 channels;
    m_swapper >> channels;
    m_channels = channels;

    uint32 samplesPerSec;
    m_swapper >> samplesPerSec;
    m_sampleRate = samplesPerSec;

    uint32 avgBytesPerSec;
    m_swapper >> avgBytesPerSec;

    uint16 blockAlign;
    m_swapper >> blockAlign;
    m_blockAlign = blockAlign;

    uint16 bitsPerSample;
    m_swapper >> bitsPerSample;
    m_bitsPerSample = bitsPerSample;

    CK_LOG_INFO("format: %d", formatTag);
    CK_LOG_INFO("channels: %d", channels);
    CK_LOG_INFO("samples per sec: %d", samplesPerSec);
    CK_LOG_INFO("avg bytes per sec: %d", avgBytesPerSec);
    CK_LOG_INFO("block align: %d", blockAlign);
    CK_LOG_INFO("bits per sample: %d", bitsPerSample);

    // validate
    bool compressed = true;
    if (formatTag == 0xfffe)
    {
        // wave_format_extensible

        uint16 cbSize;
        m_swapper >> cbSize;

        if (cbSize >= 22)
        {
            uint16 samples;
            m_swapper >> samples;

            uint32 channelMask;
            m_swapper >> channelMask;

            Guid guid;
            m_swapper >> guid;

            if (guid == k_pcmGuid)
            {
                compressed = false;
            }
        }
    }
    else if (formatTag == 3)
    {
        // wave_format_ieee_float
        m_float = true;
        compressed = false;
    }
    else if (formatTag == 1)
    {
        // wave_format_pcm
        compressed = false;
    }

    if (compressed)
    {
        CK_LOG_ERROR("Compressed wav files not supported");
        return false;
    }

    if (samplesPerSec == 0 || bitsPerSample == 0 || blockAlign == 0 || blockAlign < bitsPerSample / 8 || m_bitsPerSample > 32)
    {
        CK_LOG_ERROR("Invalid format");
        return false;
    }

    m_valid = true;
    return true;
}

bool WavReader::parseDataChunk(uint32 chunkSize)
{
    // just record its location, so we can read out of it later
    DataChunkInfo dataChunkInfo;
    dataChunkInfo.m_offset = m_file.getPos();
    dataChunkInfo.m_size = chunkSize;
    m_dataChunkInfo.push_back(dataChunkInfo);

    m_dataSize += chunkSize;
    m_file.skip(chunkSize);

    return true;
}

bool WavReader::parseCueChunk(uint32 chunkSize)
{
    uint32 numCues;
    m_swapper >> numCues;

    for (int i = 0; i < numCues; ++i)
    {
        uint32 id;
        m_swapper >> id;

        uint32 position;
        m_swapper >> position;

        // cue points have no length; length may be specified in 'adtl' chunk.
        Marker marker;
        marker.m_id = id;
        marker.m_start = (int) position;
        m_markers.push_back(marker);

        m_file.skip(16);
    }

    return true;
}

bool WavReader::parseSmplChunk(uint32 chunkSize)
{
    // skipping:
    //  manufacturer code
    //  product code
    //  sample period
    //  MIDI unity note
    //  MIDI pitch fraction
    //  SMPTE time format
    //  SMPTE time offset
    m_file.skip(sizeof(uint32) * 7);

    uint32 numLoops;
    m_swapper >> numLoops;

    uint32 samplerDataSize;
    m_swapper >> samplerDataSize;

    for (int i = 0; i < numLoops; ++i)
    {
        uint32 loopId;
        m_swapper >> loopId;

        m_file.skip(sizeof(uint32)); // loop type

        uint32 loopStart, loopEnd;
        m_swapper >> loopStart >> loopEnd;

        m_file.skip(sizeof(uint32)); // loop fraction

        uint32 loopCount;
        m_swapper >> loopCount;

        Loop loop;
        loop.m_id = loopId;
        loop.m_start = (int) loopStart;
        loop.m_end = (int) loopEnd + 1; // note different defn of loop end!
        loop.m_count = (loopCount == 0 ? -1 : (int) loopCount); // note different defn of loop count (0=infinite)

        m_loops.push_back(loop);
    }

    m_file.skip(samplerDataSize);

    return true;
}

////////////////////////////////////////
// chunk lists

bool WavReader::parseChunkList(uint32 chunkSize)
{
//    int endPos = m_file.getPos() + chunkSize;

    FourCharCode listId;
    m_swapper >> listId;

    int bytesLeft = chunkSize - 4;

    if (listId == k_adtl)
    {
        parseAdtlChunkList(bytesLeft);
    }
    else
    {
        CK_LOG_INFO("-----  skipping list chunk '%.4s' at offset %d (%d bytes)", listId.getData(), m_file.getPos(), chunkSize);
        m_file.skip(bytesLeft);
    }

    return true;
}

/*
// INFO chunk list: contains metadata like title, date, etc
bool WavReader::parseInfoChunkList(uint32 chunkSize)
{
    int endPos = m_file.getPos() + chunkSize;

    while (m_file.getPos() < endPos)
    {
        FourCharCode chunkId;
        m_swapper >> chunkId;

        uint32 size;
        m_swapper >> size;

        CK_PRINT("%.4s: ", chunkId.getData());
        const int k_bufSize = 256;
        char buf[k_bufSize];
        while (size)
        {
            int bytes = Math::min((int) size, k_bufSize);
            m_swapper.read(buf, bytes);
            CK_PRINT("%.*s", bytes, buf);
            size -= bytes;
        }
        CK_PRINT("\n");

        if (m_file.getPos() & 1)
        {
            m_file.skip(1);
        }
    }

    return true;
}
*/

bool WavReader::parseAdtlChunkList(uint32 chunkSize)
{
    int endPos = m_file.getPos() + chunkSize;

    while (m_file.getPos() < endPos)
    {
        FourCharCode chunkId;
        m_swapper >> chunkId;

        uint32 size;
        m_swapper >> size;

        if (chunkId == k_labl)
        {
            // label for a cue point

            uint32 id;
            m_swapper >> id;

            String name;
            const int k_bufSize = 256;
            char buf[k_bufSize];

            size -= sizeof(id);
            while (size)
            {
                int bytes = Math::min((int) size, k_bufSize);
                m_swapper.read(buf, bytes);
                name.append(buf, bytes);
                size -= bytes;
            }

            // store in map, since we may not have parsed the cue point yet
            m_markerNames[id] = name;
        }
        else if (chunkId == k_ltxt)
        {
            // length and maybe a label for a cue point

            uint32 id;
            m_swapper >> id;

            uint32 len;
            m_swapper >> len;

            m_markerLengths[id] = (int) len;

            // skip country, language, purpose, code page, etc; assume ASCII
            int dataSize = sizeof(uint32) + sizeof(uint16) * 4;
            m_file.skip(dataSize);

            String name;
            const int k_bufSize = 256;
            char buf[k_bufSize];

            size -= sizeof(id);
            size -= sizeof(len);
            size -= dataSize;
            if (size)
            {
                while (size)
                {
                    int bytes = Math::min((int) size, k_bufSize);
                    m_swapper.read(buf, bytes);
                    name.append(buf, bytes);
                    size -= bytes;
                }

                m_markerNames[id] = name;
            }
        }
        else
        {
            m_file.skip(size);
        }

        if (m_file.getPos() & 1)
        {
            m_file.skip(1);
        }
    }

    return true;
}



}
