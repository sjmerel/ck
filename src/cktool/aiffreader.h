#pragma once

#include "ck/core/platform.h"
#include "ck/core/filestream.h"
#include "ck/core/swapstream.h"
#include "ck/core/string.h"
#include "ck/audio/fourcharcode.h"
#include "cktool/audioreader.h"
#include <vector>

namespace Cki
{


class AiffReader : public AudioReader
{
public:
    AiffReader(const char* filename);

    virtual bool isValid() const;

    virtual int getBytes() const;
    virtual int getFrames() const;
    virtual int getChannels() const;
    virtual int getBitsPerSample() const;
    virtual int getSampleRate() const; // in Hz

    // Markers in AIFF files are always 0 length.
    virtual int getNumMarkers() const;
    virtual void getMarkerInfo(int index, const char** name, int& start, int& end) const;

    // AIFF loops are stored as references to start & end markers, so 
    // there will always be at least 2 markers if there is a loop.
    // Name will always be null.
    // There should only be 1 loop maximum.
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
    int m_dataPos; // offset into file of data
    int m_readPos; // bytes read

    // format info
    FourCharCode m_format;
    FourCharCode m_compression; // for aifc
    int m_channels;
    int m_frames;
    int m_sampleRate;
    int m_bitsPerSample;

    // marker 
    struct Marker
    {
        int m_id;
        int m_start;
        String m_name;
    };
    std::vector<Marker> m_markers;

    // loop
    struct Loop
    {
        int m_startId;
        int m_endId;
    };
    std::vector<Loop> m_loops;

    AiffReader(const AiffReader&);
    AiffReader& operator=(const AiffReader&);

    const Marker* getMarker(int id) const;

    bool parseChunk();
    bool parseSoundChunk(uint32 chunkSize);
    bool parseCommonChunk(uint32 chunkSize);
    bool parseMarkerChunk(uint32 chunkSize);
    bool parseInstrumentChunk(uint32 chunkSize);
};


}

