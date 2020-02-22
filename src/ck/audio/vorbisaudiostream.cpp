#include "ck/audio/vorbisaudiostream.h"
#include "ck/audio/audioformat.h"
#include "ck/core/binarystream.h"
#include "ck/core/mem.h"
#include <errno.h>

namespace Cki
{


namespace
{
    size_t readFunc(void* ptr, size_t size, size_t nmemb, void* datasource)
    {
        BinaryStream* stream = (BinaryStream*) datasource;

        if (!stream->isValid() || !ptr)
        {
            errno = EIO;
            return 0;
        }

        return stream->read(ptr, (int)(size*nmemb));
    }

    int seekFunc(void* datasource, ogg_int64_t offset, int whence)
    {
        BinaryStream* stream = (BinaryStream*) datasource;

        if (!stream->isValid())
        {
            return -1;
        }

        int ref = 0;
        switch (whence)
        {
            case SEEK_SET:
                ref = 0;
                break;

            case SEEK_CUR:
                ref = stream->getPos();
                break;

            case SEEK_END:
                ref = stream->getSize();
                break;
        }

        stream->setPos(ref + (int) offset);
        return 0;
    }

    long tellFunc(void* datasource)
    {
        BinaryStream* stream = (BinaryStream*) datasource;

        if (!stream->isValid())
        {
            return (long) -1;
        }

        return (long) stream->getPos();
    }

    ov_callbacks g_callbacks = 
    {
        readFunc,
        seekFunc, 
        NULL,
        tellFunc
    };
}

////////////////////////////////////////


VorbisAudioStream::VorbisAudioStream(const char* path, CkPathType pathType, int offset, int length) :
    m_readStream(path, pathType),
    m_subStream(m_readStream, offset, length),
    m_failed(false),
    m_inited(false)
{
    Mem::clear(m_ovfile);
}

VorbisAudioStream::~VorbisAudioStream()
{
    ov_clear(&m_ovfile);
}

void VorbisAudioStream::init()
{
    if (!m_failed && !m_inited)
    {
        if (ov_open_callbacks(&m_subStream, &m_ovfile, NULL, 0, g_callbacks) == 0)
        {
            vorbis_info* vinfo = ov_info(&m_ovfile, -1);
            if (vinfo->channels > 2)
            {
                // not supported
                m_failed = true;
            }
            else
            {
                m_sampleInfo.format = AudioFormat::k_pcmI16;
                m_sampleInfo.channels = (uint8) vinfo->channels;
                m_sampleInfo.sampleRate = (uint16) vinfo->rate;
                m_sampleInfo.blocks = (uint32) ov_pcm_total(&m_ovfile, -1);
                m_sampleInfo.blockBytes = (uint16) (vinfo->channels * sizeof(int16));
                m_sampleInfo.blockFrames = 1;

                m_inited = true;
            }
        }
        else
        {
            m_failed = true;
        }
    }
}

bool VorbisAudioStream::isFailed() const
{
    return m_failed;
}

int VorbisAudioStream::read(void* buf, int blocks)
{
    int blocksRead = 0;
    int16* dst = (int16*) buf;
    while (blocksRead < blocks)
    {
        int bitstream = 0;
        long bytesRead = ov_read(&m_ovfile, (char*) dst, (blocks - blocksRead)*m_sampleInfo.channels*sizeof(int16), &bitstream);
        if (bytesRead < 0)
        {
            m_failed = true;
            break;
        }

        long framesRead = bytesRead / (m_sampleInfo.channels * sizeof(int16));
        if (framesRead == 0)
        {
            break;
        }
        else
        {
            dst += framesRead * m_sampleInfo.channels;
        }
        blocksRead += framesRead;
    }
    CK_ASSERT(blocksRead <= blocks);

    return blocksRead;
}

int VorbisAudioStream::getNumBlocks() const
{
    return m_sampleInfo.blocks;
}

void VorbisAudioStream::setBlockPos(int block)
{
    block = Math::clamp(block, 0, m_sampleInfo.blocks);
    ov_pcm_seek(&m_ovfile, block); // TODO ov_pcm_seek_lap?
}

int VorbisAudioStream::getBlockPos() const
{
    return (int) ov_pcm_tell((OggVorbis_File*) &m_ovfile);
}



}
    
