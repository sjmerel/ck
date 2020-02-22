#import "ck/objc/sound.h"
#import "ck/objc/bank.h"
#import "ck/api/objc/bank_p.h"
#import "ck/api/objc/mixer_p.h"
#import "ck/api/objc/effectbus_p.h"
#import "ck/api/objc/nsstringref.h"
#import "ck/api/objc/proxy.h"
#import "ck/api/objc/sound_p.h"
#import "ck/core/path.h"
#import "ck/sound.h"

@implementation CkoSound
{
    CkSound* m_impl;
}


- (void) setMixer:(CkoMixer*)mixer
{
    m_impl->setMixer(mixer ? [mixer impl] : NULL);
}

- (CkoMixer*) mixer
{
    CkMixer* mixerImpl = m_impl->getMixer();
    return CkoMixerProxyGet(mixerImpl);
}

- (BOOL) ready
{
    return m_impl->isReady();
}

- (BOOL) failed
{
    return m_impl->isFailed();
}

- (void) play
{
    m_impl->play();
}

- (void) stop
{
    m_impl->stop();
}

- (BOOL) playing
{
    return m_impl->isPlaying();
}

- (void) setPaused:(BOOL)paused
{
    m_impl->setPaused(paused);
}

- (BOOL) paused
{
    return m_impl->isPaused();
}

- (void) setLoop:(int)startFrame endFrame:(int)endFrame
{
    m_impl->setLoop(startFrame, endFrame);
}

- (int) loopStart
{
    int loopStart, loopEnd;
    m_impl->getLoop(loopStart, loopEnd);
    return loopStart;
}

- (int) loopEnd
{
    int loopStart, loopEnd;
    m_impl->getLoop(loopStart, loopEnd);
    return loopEnd;
}

- (void) setLoopCount:(int)loopCount
{
    m_impl->setLoopCount(loopCount);
}

- (int) loopCount
{
    return m_impl->getLoopCount();
}

- (int) currentLoop
{
    return m_impl->getCurrentLoop();
}

- (void) releaseLoop
{
    m_impl->releaseLoop();
}

- (BOOL) loopReleased
{
    return m_impl->isLoopReleased();
}

- (void) setPlayPosition:(int)frame
{
    m_impl->setPlayPosition(frame);
}

- (int) playPosition
{
    return m_impl->getPlayPosition();
}

- (void) setPlayPositionMs:(float)ms
{
    m_impl->setPlayPositionMs(ms);
}

- (float) playPositionMs
{
    return m_impl->getPlayPositionMs();
}

- (void) setVolume:(float)volume
{
    m_impl->setVolume(volume);
}

- (float) volume
{
    return m_impl->getVolume();
}

- (float) mixedVolume
{
    return m_impl->getMixedVolume();
}

- (void) setPan:(float)pan
{
    m_impl->setPan(pan);
}

- (float) pan
{
    return m_impl->getPan();
}

- (void) setPanMatrixLL:(float)ll LR:(float)lr RL:(float)rl RR:(float)rr
{
    m_impl->setPanMatrix(ll, lr, rl, rr);
}

- (void) getPanMatrixLL:(float*)ll LR:(float*)lr RL:(float*)rl RR:(float*)rr
{
    m_impl->getPanMatrix(*ll, *lr, *rl, *rr);
}

- (void) setPitchShift:(float)halfSteps
{
    m_impl->setPitchShift(halfSteps);
}

- (float) pitchShift
{
    return m_impl->getPitchShift();
}

- (void) setSpeed:(float)speed
{
    m_impl->setSpeed(speed);
}

- (float) speed
{
    return m_impl->getSpeed();
}

- (void) setNextSound:(CkoSound*)next
{
    m_impl->setNextSound(next ? [next impl] : NULL);
}

- (CkoSound*) nextSound
{
    CkSound* soundImpl = m_impl->getNextSound();
    return CkoSoundProxyGet(soundImpl);
}

- (int) length
{
    return m_impl->getLength();
}

- (float) lengthMs
{
    return m_impl->getLengthMs();
}

- (int) sampleRate
{
    return m_impl->getSampleRate();
}

- (int) channels
{
    return m_impl->getChannels();
}

- (void) setEffectBus:(CkoEffectBus*)effectBus
{
    m_impl->setEffectBus(effectBus ? [effectBus impl] : NULL);
}

- (CkoEffectBus*) effectBus
{
    return (CkoEffectBus*) m_impl->getEffectBus();
}

- (void) setThreeDEnabled:(BOOL)enabled
{
    m_impl->set3dEnabled(enabled);
}

- (BOOL) threeDEnabled
{
    return m_impl->is3dEnabled();
}

- (BOOL) isVirtual
{
    return m_impl->isVirtual();
}

- (void) set3dPositionX:(float)x y:(float)y z:(float)z
{
    m_impl->set3dPosition(x, y, z);
}

- (void) get3dPositionX:(float*)x y:(float*)y z:(float*)z
{
    m_impl->get3dPosition(*x, *y, *z);
}

- (void) set3dVelocityX:(float)x y:(float)y z:(float)z
{
    m_impl->set3dVelocity(x, y, z);
}

- (void) get3dVelocityX:(float*)x y:(float*)y z:(float*)z
{
    m_impl->get3dVelocity(*x, *y, *z);
}

+ (void) set3dListenerPositionEyeX:(float)eyeX eyeY:(float)eyeY eyeZ:(float)eyeZ
           lookAtX:(float)lookAtX lookAtY:(float)lookAtY lookAtZ:(float)lookAtZ
           upX:(float)upX upY:(float)upY upZ:(float)upZ
{
    CkSound::set3dListenerPosition(eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ, upX, upY, upZ);
}

+ (void) get3dListenerPositionEyeX:(float*)eyeX eyeY:(float*)eyeY eyeZ:(float*)eyeZ
           lookAtX:(float*)lookAtX lookAtY:(float*)lookAtY lookAtZ:(float*)lookAtZ
           upX:(float*)upX upY:(float*)upY upZ:(float*)upZ
{
    CkSound::get3dListenerPosition(*eyeX, *eyeY, *eyeZ, *lookAtX, *lookAtY, *lookAtZ, *upX, *upY, *upZ);
}

+ (void) set3dListenerVelocityX:(float)x y:(float)y z:(float)z
{
    CkSound::set3dListenerVelocity(x, y, z);
}

+ (void) get3dListenerVelocityX:(float*)x y:(float*)y z:(float*)z
{
    CkSound::get3dListenerVelocity(*x, *y, *z);
}

+ (void) set3dSoundSpeed:(float)speed
{
    CkSound::set3dSoundSpeed(speed);
}

+ (float) get3dSoundSpeed
{
    return CkSound::get3dSoundSpeed();
}

+ (float) getSoundSpeedCentimetersPerSecond
{
    return CkSound::k_soundSpeed_CentimetersPerSecond;
}

+ (float) getSoundSpeedMetersPerSecond
{
    return CkSound::k_soundSpeed_MetersPerSecond;
}

+ (float) getSoundSpeedInchesPerSecond
{
    return CkSound::k_soundSpeed_InchesPerSecond;
}

+ (float) getSoundSpeedFeetPerSecond
{
    return CkSound::k_soundSpeed_FeetPerSecond;
}


+ (void) set3dAttenuationMode:(CkAttenuationMode)mode nearDist:(float)nearDist farDist:(float)farDist farVol:(float)farVol
{
    CkSound::set3dAttenuation(mode, nearDist, farDist, farVol);
}

+ (void) get3dAttenuationMode:(CkAttenuationMode*)mode nearDist:(float*)nearDist farDist:(float*)farDist farVol:(float*)farVol
{
    CkSound::get3dAttenuation(*mode, *nearDist, *farDist, *farVol);
}

+ newBankSound:(CkoBank*)bank index:(int)index
{
    CkSound* soundImpl = CkSound::newBankSound([bank impl], index);
    return CkoSoundProxyGet(soundImpl);
}

+ newBankSound:(CkoBank*)bank name:(NSString*)name
{
    NsStringRef str(name);
    CkSound* soundImpl = CkSound::newBankSound([bank impl], str.getChars());
    return CkoSoundProxyGet(soundImpl);
}

+ newStreamSound:(NSString*)path
{
    return [CkoSound newStreamSound:path pathType:kCkPathType_Default];
}

+ newStreamSound:(NSString*)path pathType:(CkPathType)pathType
{
    NsStringRef str(path);
    CkSound* impl = CkSound::newStreamSound(str.getChars(), pathType);
    return CkoSoundProxyGet(impl);
}

+ newStreamSound:(NSString*)path pathType:(CkPathType)pathType offset:(int)offset length:(int)length extension:(NSString*)extension
{
    NsStringRef pathStr(path);
    NsStringRef extStr(extension);
    CkSound* impl = CkSound::newStreamSound(pathStr.getChars(), pathType, offset, length, extStr.getChars());
    return CkoSoundProxyGet(impl);
}

#if CK_PLATFORM_IOS
+ newAssetStreamSound:(NSURL*)url
{
    NsStringRef str(url.absoluteString);
    CkSound* impl = CkSound::newAssetStreamSound(str.getChars());
    return CkoSoundProxyGet(impl);
}
#endif


////////////////////////////////////////
// overrides

- (void) dealloc
{
    if (m_impl)
    {
        CkoSoundProxyRemove(m_impl);
        m_impl->destroy();
        m_impl = NULL;
    }
    [super dealloc];
}

@end

////////////////////////////////////////
// private

@implementation CkoSound (hidden)

- (void) destroy
{
    if (m_impl)
    {
        m_impl = NULL;
        [self release];
    }
}

+ (CkoSound*) createWithImpl:(CkSound*)impl
{
    CkoSound* sound = [[CkoSound alloc] init];
    if (sound)
    {
        sound->m_impl = impl;
    }
    return sound;
}

- (CkSound*) impl
{
    return m_impl;
}

@end
