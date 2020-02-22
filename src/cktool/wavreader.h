#pragma once

#include "ck/core/platform.h"
#include "ck/core/filestream.h"
#include "ck/core/swapstream.h"
#include "ck/core/string.h"
#include "cktool/audioreader.h"
#include <vector>
#include <map>

namespace Cki
{


class WavReader : public AudioReader
{
public:
    WavReader(const char* filename);

    virtual bool isValid() const;

    virtual int getBytes() const;
    virtual int getFrames() const;
    virtual int getChannels() const;
    virtual int getBitsPerSample() const;
    virtual int getSampleRate() const; // in Hz

    // Markers may have length ("regions").
    virtual int getNumMarkers() const;
    virtual void getMarkerInfo(int index, const char** name, int& start, int& end) const;

    virtual int getNumLoops() const;
    virtual void getLoopInfo(int index, const char** name, int& start, int& end, int& count) const;

    // Read audio data; return number of samples read.
    virtual int read(int16*, int samples);

    virtual bool isAtEnd() const;

private:
    FileStream m_file;
    SwapStream m_swapper;
    bool m_valid;
    int m_dataSize;
    int m_readPos; // bytes read
    int m_curChunk;

    // format info
    int m_channels;
    int m_sampleRate;
    int m_blockAlign;
    int m_bitsPerSample;
    bool m_float;

    struct DataChunkInfo
    {
        int m_offset;
        int m_size;
    };
    std::vector<DataChunkInfo> m_dataChunkInfo;

    // marker (or region)
    struct Marker
    {
        uint32 m_id;
        int m_start;
    };
    std::vector<Marker> m_markers;

    // loop
    struct Loop
    {
        uint32 m_id;
        int m_start;
        int m_end;
        int m_count;
    };
    std::vector<Loop> m_loops;

    // additional data:
    std::map<uint32, String> m_markerNames;
    std::map<uint32, int> m_markerLengths;

    WavReader(const WavReader&);
    WavReader& operator=(const WavReader&);

    const char* getMarkerName(uint32 id) const;
    int getMarkerLength(uint32 id) const;

    bool parseChunk();
    bool parseDataChunk(uint32 chunkSize);
    bool parseFmtChunk(uint32 chunkSize);
    bool parseCueChunk(uint32 chunkSize);
    bool parseSmplChunk(uint32 chunkSize);

    bool parseChunkList(uint32 chunkSize);
    bool parseAdtlChunkList(uint32 chunkSize);

    int readImpl(void*, int bytes);
};


}
