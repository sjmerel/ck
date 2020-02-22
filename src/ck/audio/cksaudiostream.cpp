#include "ck/audio/cksaudiostream.h"
#include "ck/audio/fileheader.h"
#include "ck/audio/filetype.h"
#include "ck/core/swapstream.h"
#include "ck/core/debug.h"
#include "ck/core/logger.h"

namespace Cki
{


CksAudioStream::CksAudioStream(const char* path, CkPathType pathType, int offset, int length) :
    m_dataPos(-1),
    m_failed(false),
    m_readStream(path, pathType),
    m_subStream(m_readStream, offset, length)
{
}

void CksAudioStream::init()
{
    if (!m_failed && m_dataPos < 0)
    {
        // read header info
        SwapStream stream(m_subStream);

        FileHeader fileHeader;
        stream >> fileHeader;

        if (fileHeader.marker != FileHeader::k_marker)
        {
            CK_LOG_ERROR("Invalid file marker: \"%.4s\" (expecting \"%.4s\")", fileHeader.marker.getData(), FileHeader::k_marker.getData());
            m_failed = true;
            return;
        }
        if (!(fileHeader.targets & Target::k_current))
        {
            CK_LOG_ERROR("Invalid file target mask: %x (does not include current target %x)", fileHeader.targets, Target::k_current);
            m_failed = true;
            return;
        }
        if (fileHeader.fileType != FileType::k_stream)
        {
            CK_LOG_ERROR("Invalid file type: %d (expecting %d)", fileHeader.fileType, FileType::k_stream);
            m_failed = true;
            return;
        }
        if (fileHeader.fileVersion != FileHeader::k_version)
        {
            CK_LOG_ERROR("Invalid file version: %d (expecting %d)", fileHeader.fileVersion, FileHeader::k_version);
            if (fileHeader.fileVersion > FileHeader::k_version)
            {
                CK_LOG_ERROR(" Are you linking to an older version of the Cricket library?");
            }
            else
            {
                CK_LOG_ERROR(" You may need to rebuild your streams with the latest version of cktool.");
            }
            m_failed = true;
            return;
        }

        stream >> m_sampleInfo;
        m_dataPos = m_subStream.getPos();
    }
}

bool CksAudioStream::isFailed() const
{
    return m_failed;
}

int CksAudioStream::read(void* buf, int blocks)
{
    CK_ASSERT(m_dataPos >= 0 && !m_failed);
    int bytesRead = m_subStream.read(buf, blocks * m_sampleInfo.blockBytes);
    CK_ASSERT(bytesRead % m_sampleInfo.blockBytes == 0);
    return bytesRead / m_sampleInfo.blockBytes;
}

int CksAudioStream::getNumBlocks() const
{
    CK_ASSERT(m_dataPos >= 0 && !m_failed);
    int numBytes = m_subStream.getSize() - m_dataPos;
    CK_ASSERT(numBytes % m_sampleInfo.blockBytes == 0);
    return numBytes / m_sampleInfo.blockBytes;
}

void CksAudioStream::setBlockPos(int block)
{
    CK_ASSERT(m_dataPos >= 0 && !m_failed);
    m_subStream.setPos(block * m_sampleInfo.blockBytes + m_dataPos);
}

int CksAudioStream::getBlockPos() const
{
    CK_ASSERT(m_dataPos >= 0 && !m_failed);
    int bytePos = m_subStream.getPos() - m_dataPos;
    CK_ASSERT(bytePos % m_sampleInfo.blockBytes == 0);
    int blockPos = bytePos / m_sampleInfo.blockBytes;
    return Math::min(blockPos, getNumBlocks());
}



}
