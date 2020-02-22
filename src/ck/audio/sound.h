#pragma once

#include "ck/core/platform.h"
#include "ck/core/proxied.h"
#include "ck/core/list.h"
#include "ck/core/allocatable.h"
#include "ck/core/vector3.h"
#include "ck/core/timer.h"
#include "ck/pathtype.h"
#include "ck/sound.h"
#include "ck/audio/volumematrix.h"
#include "ck/audio/effectbus.h"

namespace Cki
{

class Sound;
typedef List<Sound, 0> SoundList;
typedef List<Sound, 1> SoundMixerList;
typedef List<Sound, 2> SoundPrevList;


class Bank;
class Mixer;
class SourceNode;

class Sound : 
    public CkSound,
    public Allocatable, 
    public Proxied<Sound>,
    public SoundList::Node,     // for update()
    public SoundMixerList::Node,// for Mixer's list of child sounds
    public SoundPrevList::Node  // for next sound
{
public:
    Sound();
    virtual ~Sound();

    virtual void destroy();

    void setMixer(CkMixer*);
    CkMixer* getMixer();

    virtual void setEffectBus(CkEffectBus*) = 0;
    virtual EffectBus* getEffectBus() = 0;

    virtual bool isReady() const;
    virtual bool isFailed() const = 0;

    virtual void play();
    virtual void stop();
    virtual bool isPlaying() const = 0; 

    virtual void setPaused(bool);
    virtual bool isPaused() const;
    virtual bool getMixedPauseState() const;

    virtual void setLoop(int startFrame, int endFrame) = 0;
    virtual void getLoop(int& startFrame, int& endFrame) const = 0;
    virtual void setLoopCount(int) = 0;
    virtual int getLoopCount() const = 0;
    virtual int getCurrentLoop() const = 0;
    virtual void releaseLoop() = 0;
    virtual bool isLoopReleased() const = 0;

    virtual void setPlayPosition(int) = 0;
    virtual void setPlayPositionMs(float) = 0;
    virtual int getPlayPosition() const = 0;
    virtual float getPlayPositionMs() const = 0;

    void setVolume(float);
    float getVolume() const;
    float getMixedVolume() const; // affected by mixers (but not 3D attenuation)

    void setPan(float pan);
    float getPan() const;

    void setPanMatrix(float ll, float lr, float rl, float rr);
    void getPanMatrix(float& ll, float& lr, float& rl, float& rr) const;

    void setSpeed(float speed);
    float getSpeed() const;

    void setPitchShift(float halfSteps);
    float getPitchShift() const;

    virtual void setNextSound(CkSound*);
    virtual CkSound* getNextSound() const;

    virtual int getLength() const = 0;
    virtual float getLengthMs() const = 0;
    virtual int getSampleRate() const = 0;
    virtual int getChannels() const = 0;

    void set3dEnabled(bool);
    bool is3dEnabled() const;
    bool isVirtual() const;
    void set3dPosition(float x, float y, float z);
    void get3dPosition(float& x, float& y, float& z) const;
    void set3dVelocity(float vx, float vy, float vz);
    void get3dVelocity(float& vx, float& vy, float& vz) const;
    static void set3dListenerPosition(float eyeX, float eyeY, float eyeZ,
                                      float lookAtX, float lookAtY, float lookAtZ,
                                      float upX, float upY, float upZ);
    static void get3dListenerPosition(float& eyeX, float& eyeY, float& eyeZ,
                                      float& lookAtX, float& lookAtY, float& lookAtZ,
                                      float& upX, float& upY, float& upZ);
    static void set3dListenerVelocity(float vx, float vy, float vz);
    static void get3dListenerVelocity(float& vx, float& vy, float& vz);
    static void set3dAttenuation(CkAttenuationMode mode, float nearDist, float farDist, float farVol);
    static void get3dAttenuation(CkAttenuationMode& mode, float& nearDist, float& farDist, float& farVol);

    static void set3dSoundSpeed(float);
    static float get3dSoundSpeed();

    static void updateAll();

    // should be called only by mixer or subclasses:
    virtual void updateVolumeAndPan() = 0;
    virtual void updatePaused() = 0;

    virtual SourceNode* getSourceNode();

    static Sound* newBankSound(Bank* bank, int index);
    static Sound* newBankSound(Bank* bank, const char* name);
    static Sound* newStreamSound(const char* filename, CkPathType, int offset, int length, const char* extension);
    static void setCustomStreamHandler(CustomStreamFunc, void* data);
#if CK_PLATFORM_IOS
    static Sound* newAssetStreamSound(const char* url);
#endif

protected:
    float m_volume;
    bool m_volumeSet; // if true, volume was set explicitly by API (overriding values from data file)
    float m_pan;
    VolumeMatrix m_panMatrix;
    bool m_panMatrixExplicit; // if true, pan matrix was set explicitly, rather than from pan value
    bool m_panSet; // if true, pan was set explicitly by API (overriding values from data file)
    bool m_paused;
    float m_speed;
    Mixer* m_mixer;
    Sound* m_nextSound;

    void destroyImpl();
    void removePrevSounds();

    float getFinalVolume() const;
    float getFinalPan() const;
    void  getFinalVolumeMatrix(VolumeMatrix&) const;
    float getFinalSpeed() const;

    virtual void update();
    virtual void updateSpeed() = 0;
    virtual void updateVirtual() = 0;

    virtual void playSub() = 0;
    virtual void stopSub() = 0;
    virtual void updateSub() = 0;
    virtual bool isReadySub() const = 0;

private:
    struct AttenParams
    {
        AttenParams();
        void set(CkAttenuationMode mode, float nearDist, float farDist, float farVol);

        CkAttenuationMode m_mode;
        float m_nearDist;
        float m_farDist;
        float m_farVol;
        float m_consts[2]; // cached constants
    };

    bool m_3d;
    float m_3dVol;
    float m_3dPan;
    float m_3dSpeed;
    Vector3 m_3dPos;
    Vector3 m_3dVel;
    bool m_virtual;
//    AttenParams m_3dAtten;
    Timer m_virtualTimer;
    float m_virtualStopMs;
    bool m_playWhenReady;
    SoundPrevList m_prevSounds;

    static SoundList s_soundList;
    static Vector3 s_3dListenerPos;
    static Vector3 s_3dListenerVel;
    static Vector3 s_3dListenerLookAt;
    static Vector3 s_3dListenerUp;
    static AttenParams s_3dAtten;
    static float s_3dSoundSpeed;

    static CustomStreamFunc s_handler;
    static void* s_handlerData;

    void update3d();
    void setVirtual(bool);
    void startVirtualTimer();
    float getMsLeft();
};



}

