#include "aiffreader.h"
#include "ck/core/debug.h"
#include "ck/core/logger.h"
#include "ck/core/math.h"

namespace Cki
{

namespace
{

    const FourCharCode k_form('F', 'O', 'R', 'M');
    const FourCharCode k_aiff('A', 'I', 'F', 'F');
    const FourCharCode k_aifc('A', 'I', 'F', 'C');

    const FourCharCode k_comm('C', 'O', 'M', 'M');
    const FourCharCode k_ssnd('S', 'S', 'N', 'D');
    const FourCharCode k_mark('M', 'A', 'R', 'K');
    const FourCharCode k_inst('I', 'N', 'S', 'T');

    // aiff-c compression
    const FourCharCode k_none('N', 'O', 'N', 'E');
    const FourCharCode k_fl32('f', 'l', '3', '2');
    const FourCharCode k_FL32('F', 'L', '3', '2');
    const FourCharCode k_sowt('s', 'o', 'w', 't');
}



AiffReader::AiffReader(const char* filename) :
    m_file(filename, FileStream::k_read),
    m_swapper(m_file, ByteOrder::k_bigEndian), 
    m_valid(false),
    m_dataSize(0),
    m_dataPos(0),
    m_readPos(0),
    m_format(),
    m_compression(),
    m_channels(0),
    m_sampleRate(0),
    m_bitsPerSample(0)
{
    if (!m_file.isValid() || m_file.getSize() < 12)
    {
        return;
    }

    // FORM chunk
    FourCharCode chunkId;
    m_swapper >> chunkId;
    if (chunkId != k_form)
    {
        return;
    }

    uint32 chunkSize;
    m_swapper >> chunkSize;
    if (chunkSize > (uint32) m_file.getSize() - 8)
    {
        return;
    }

    m_swapper >> m_format;
    if (m_format != k_aiff && m_format != k_aifc)
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

    // m_valid is set to true if we read a format chunk; make sure we have data too.
    m_valid &= (m_dataPos > 0);
    m_valid &= (m_dataSize % (m_bitsPerSample / 8 * m_channels) == 0);

    if (m_valid)
    {
        // seek to beginning of data
        m_file.setPos(m_dataPos);

        if (m_compression == k_fl32 || m_compression == k_FL32)
        {
            // calculate normalization factor
            int pos = m_file.getPos();

            int samples = m_dataSize / sizeof(float);
            for (int i = 0; i < samples; ++i)
            {
                float tmp;
                if (m_file.read(&tmp, sizeof(float)) == sizeof(float))
                {
                    if (ByteOrder::k_native != ByteOrder::k_bigEndian)
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
            m_readPos = 0;
        }
    }
}

bool AiffReader::isValid() const
{
    return m_valid;
}

int AiffReader::getBytes() const
{
    return m_dataSize;
}

int AiffReader::getFrames() const
{
    return m_frames;
}

int AiffReader::getChannels() const
{
    return m_channels;
}

int AiffReader::getBitsPerSample() const
{
    return m_bitsPerSample;
}

int AiffReader::getSampleRate() const
{
    return m_sampleRate;
}

int AiffReader::getNumMarkers() const
{
    return (int) m_markers.size();
}

void AiffReader::getMarkerInfo(int index, const char** name, int& start, int& end) const
{
    const Marker& marker = m_markers[index];

    if (name)
    {
        *name = marker.m_name.getBuffer();
    }

    start = end = marker.m_start;
}

int AiffReader::getNumLoops() const
{
    return (int) m_loops.size();
}

void AiffReader::getLoopInfo(int index, const char** name, int& start, int& end, int& count) const
{
    const Loop& loop = m_loops[index];

    if (name)
    {
        *name = NULL; // no names
    }

    const Marker* startMarker = getMarker(loop.m_startId);
    if (startMarker)
    {
        start = startMarker->m_start;
    }
    else
    {
        CK_LOG_WARNING("Invalid loop start marker id; loop start is set to 0");
        start = 0;
    }

    const Marker* endMarker = getMarker(loop.m_endId);
    if (endMarker)
    {
        end = endMarker->m_start;
    }
    else
    {
        CK_LOG_WARNING("Invalid loop end marker id; loop end is set to end of data");
        end = m_frames;
    }

    count = -1;
}


int AiffReader::read(int16* buf, int samples)
{
    if (!m_valid)
    {
        return 0;
    }

    if (m_bitsPerSample == 16 && ByteOrder::k_native == ByteOrder::k_bigEndian)
    {
        int bytesRead = m_file.read(buf, samples * sizeof(int16));
        return bytesRead / sizeof(int16);
    }
    else
    {
        byte tmp[4]; // holds up to 32 bits
        int blockBytes = ((m_bitsPerSample-1) / 8) + 1;
        for (int i = 0; i < samples; ++i)
        {
            // TODO nonstandard bit depth (e.g. 18 bits in a 24-bit container)
            // TODO sample bytes < block bytes (e.g. 24 bits in a 32-bit container)
            int bytesRead = m_file.read(tmp, blockBytes);
            m_readPos += bytesRead;
            if (bytesRead == blockBytes)
            {
                if (m_compression == k_sowt)
                {
                    if (ByteOrder::k_native != ByteOrder::k_littleEndian)
                    {
                        swapSample(tmp, m_bitsPerSample);
                    }
                }
                else
                {
                    if (ByteOrder::k_native != ByteOrder::k_bigEndian)
                    {
                        swapSample(tmp, m_bitsPerSample);
                    }
                }

                if (m_compression == k_fl32 || m_compression == k_FL32)
                {
                    *buf++ = convertFloatSample(tmp);
                }
                else
                {
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

bool AiffReader::isAtEnd() const
{
    return m_readPos >= m_dataSize;
}

////////////////////////////////////////

const AiffReader::Marker* AiffReader::getMarker(int id) const
{
    for (int i = 0; i < m_markers.size(); ++i)
    {
        if (m_markers[i].m_id == id)
        {
            return &m_markers[i];
        }
    }

    return NULL;
}

bool AiffReader::parseChunk()
{
    FourCharCode chunkId;
    m_swapper >> chunkId;

    uint32 chunkSize;
    m_swapper >> chunkSize;

    int startPos = m_file.getPos();
    int endPos = startPos + chunkSize;
    if (endPos & 1)
    {
        endPos += 1; // pad byte
    }

    bool success = true;
    if (chunkId == k_comm)
    {
        success = parseCommonChunk(chunkSize);
    }
    else if (chunkId == k_ssnd)
    {
        success = parseSoundChunk(chunkSize);
    }
    else if (chunkId == k_mark)
    {
        success = parseMarkerChunk(chunkSize);
    }
    else if (chunkId == k_inst)
    {
        success = parseInstrumentChunk(chunkSize);
    }
    else
    {
        CK_LOG_INFO("skipping chunk '%.4s' at offset %d", chunkId.getData(), m_file.getPos());
        m_file.skip(chunkSize);
    }

    // skip unread bytes
    int pos = m_file.getPos();
    CK_ASSERT(pos <= endPos);
    if (pos < endPos)
    {
        m_file.skip(endPos - pos);
    }

    if ((endPos & 1) && (endPos < m_file.getSize()))
    {
        // skip pad byte
        m_file.skip(1);
    }

    return success;
}

bool AiffReader::parseCommonChunk(uint32 chunkSize)
{
    uint16 channels;
    m_swapper >> channels;
    m_channels = channels;

    uint32 frames;
    m_swapper >> frames;
    m_frames = frames;

    int16 bitsPerSample;
    m_swapper >> bitsPerSample;
    m_bitsPerSample = bitsPerSample;

    // sample rate is an IEEE 754 extended float (10 bytes)
    // see http://muratnkonar.com/aiff/index.html
    m_swapper.skip(1);
    int8 exp;
    m_swapper >> exp;
    uint32 mantissa;
    m_swapper >> mantissa;
    exp = 30 - exp;

    uint32 last = 0;
    while (exp--)
    {
        last = mantissa;
        mantissa >>= 1;
    }
    if (last & 0x00000001) mantissa++;
    m_sampleRate = mantissa;

    m_swapper.skip(4); // remaining bytes of extended float


    if (m_format == k_aifc)
    {
        m_swapper >> m_compression;

        // compression name
        byte strLen;
        m_swapper >> strLen;
        if (strLen % 2 == 0)
        {
            ++strLen; // pstring length must be even
        }
        m_swapper.skip(strLen);

        CK_LOG_INFO("compression: %.4s", m_compression.getData());

        if (m_compression != k_none && 
            m_compression != k_fl32 &&
            m_compression != k_FL32 &&
            m_compression != k_sowt)
        {
            CK_LOG_ERROR("Invalid compression");
            return false;
        }
    }

    CK_LOG_INFO("channels: %d", channels);
    CK_LOG_INFO("frames: %d", frames);
    CK_LOG_INFO("bits per sample: %d", bitsPerSample);
    CK_LOG_INFO("sample rate: %d", m_sampleRate);

    if (m_sampleRate == 0 || bitsPerSample == 0 || m_bitsPerSample > 32)
    {
        CK_LOG_ERROR("Invalid format");
        return false;
    }

    m_valid = true;
    return true;
}

bool AiffReader::parseSoundChunk(uint32 chunkSize)
{
    uint32 offset;
    m_swapper >> offset;
    uint32 blockSize;
    m_swapper >> blockSize;

    m_dataPos = m_file.getPos() + offset;
    m_dataSize += chunkSize - offset - sizeof(blockSize) - sizeof(offset);

    return true;
}

bool AiffReader::parseMarkerChunk(uint32 chunkSize)
{
    uint16 numMarkers;
    m_swapper >> numMarkers;

    for (int i = 0; i < numMarkers; ++i)
    {
        int16 id;
        m_swapper >> id;

        uint32 position;
        m_swapper >> position;

        uint8 nameSize;
        m_swapper >> nameSize;

        String name;
        name.resize(nameSize);
        m_swapper.read(name.getBuffer(), nameSize);

        Marker marker;
        marker.m_id = id;
        marker.m_start = position;
        marker.m_name = name;
        m_markers.push_back(marker);

        if (m_file.getPos() & 1)
        {
            m_file.skip(1);
        }
    }

    return true;
}

bool AiffReader::parseInstrumentChunk(uint32 chunkSize)
{
    m_file.skip(8); // skip MIDI note info, etc

    // sustain loop:
    int16 playMode;
    m_swapper >> playMode; 

    int16 startId, endId;
    m_swapper >> startId >> endId;

    if (playMode != 0)
    {
        Loop loop;
        loop.m_startId = startId;
        loop.m_endId = endId;
        m_loops.push_back(loop);
    }

    // skip release loop
    m_file.skip(6);

    return true;
}


}

