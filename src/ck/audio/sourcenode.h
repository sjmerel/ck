#pragma once

#include "ck/core/platform.h"
#include "ck/audio/audionode.h"
#include "ck/audio/resampler.h"

namespace Cki
{

class AudioSource;
class MixNode;
class Decoder;

class SourceNode : public AudioNode
{
public:
    SourceNode();
    void init(AudioSource*);

    virtual BufferHandle processFloat(int frames, bool post, bool& needsPost);
    virtual BufferHandle processFixed(int frames, bool post, bool& needsPost);

    enum 
    {
        k_setLoopCount,
        k_releaseLoop,

        k_setLoop,
        k_setNextFrame,

        k_setDecoder,

        k_setOutput,
        k_connect,
        k_disconnect,

        k_setNext,

        k_setSpeed,

        // volume matrix
        k_setVolumeL, // LL, LR components
        k_setVolumeR, // RL, RR components

        k_setPlaying,
        k_setPaused,

        k_setPlayPosition,
        k_setPlayPositionMs,
    };

    virtual void execute(int cmdId, CommandParam param0, CommandParam param1);

    int getPlayId() const { return m_playId; }
    int getDoneId() const { return m_doneId; }

#if CK_DEBUG
    virtual void print(int level);
#endif

private:
    AudioSource* m_source;
    Decoder* m_decoder;
    SourceNode* m_next;
    MixNode* m_output;
    bool m_connected;
    VolumeMatrix m_volMat;
    VolumeMatrix m_targetVolMat;
    int m_playId; // 0 if not playing, else id from GraphSound class, or -1
    int m_doneId;
    int m_outOffset;
    bool m_paused;
    int m_nextFrame;
    float m_nextMs;
    float m_speed;
    Resampler m_resampler;

    template <typename T> BufferHandle process(int frames, bool post, bool& needsPost);
    void connect();
    void disconnect();
};


}
