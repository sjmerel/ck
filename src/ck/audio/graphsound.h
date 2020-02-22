#pragma once

#include "ck/core/platform.h"
#include "ck/core/list.h"
#include "ck/core/deletable.h"
#include "ck/audio/sound.h"
#include "ck/audio/sourcenode.h"
#include "ck/audio/decoderbuf.h"

namespace Cki
{

class MixNode;
class AudioSource;

class GraphSound;
typedef List<GraphSound, 1> EffectBusSoundList;

class GraphSound : 
    public Sound, 
    public Deletable, 
    public EffectBusSoundList::Node   // for EffectBus's list of input sounds
{
public:
    GraphSound();

    virtual void destroy();

    virtual bool isFailed() const;

    virtual bool isPlaying() const; 

    virtual void setLoop(int startFrame, int endFrame);
    virtual void getLoop(int& startFrame, int& endFrame) const;
    virtual void setLoopCount(int);
    virtual int getLoopCount() const;
    virtual int getCurrentLoop() const;
    virtual void releaseLoop();
    virtual bool isLoopReleased() const;

    virtual int getPlayPosition() const;
    virtual float getPlayPositionMs() const;

    virtual void setNextSound(CkSound*);

    virtual int getLength() const;
    virtual float getLengthMs() const;
    virtual int getSampleRate() const;
    virtual int getChannels() const;

    virtual void setEffectBus(CkEffectBus*);
    virtual EffectBus* getEffectBus();

    virtual void updateVolumeAndPan(); // should be called only by mixer!

    virtual SourceNode* getSourceNode();

protected:
    void subInit(AudioSource*); // call in subclass ctor

    virtual void playSub();
    virtual void updateSub();
    virtual void stopSub();
    virtual bool isReadySub() const;

    virtual void updatePaused();
    virtual void updateSpeed();
    virtual void updateVirtual();

    virtual void updateSeek() {}

protected:
    AudioSource* m_source;
    DecoderBuf m_decoder;
    SourceNode m_sourceNode;
    MixNode* m_outMixNode;
    EffectBus* m_fx;
    bool m_inited;
    bool m_playing;

private:
    int m_loopCount;
    int m_loopStart;
    int m_loopEnd;
    int m_playId; // identifies the last play

    void init();
    void updateMixNode();
};


}
