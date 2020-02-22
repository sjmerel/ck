#include "ck/sound.h"
#include "ck/audio/sound.h"


void CkSound::set3dListenerPosition(float eyeX, float eyeY, float eyeZ,
                                    float lookAtX, float lookAtY, float lookAtZ,
                                    float upX, float upY, float upZ)
{
    Cki::Sound::set3dListenerPosition(eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ, upX, upY, upZ);
}

void CkSound::get3dListenerPosition(float& eyeX, float& eyeY, float& eyeZ,
                                    float& lookAtX, float& lookAtY, float& lookAtZ,
                                    float& upX, float& upY, float& upZ)
{
    Cki::Sound::get3dListenerPosition(eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ, upX, upY, upZ);
}

void CkSound::set3dListenerVelocity(float vx, float vy, float vz)
{
    Cki::Sound::set3dListenerVelocity(vx, vy, vz);
}

void CkSound::get3dListenerVelocity(float& vx, float& vy, float& vz)
{
    Cki::Sound::get3dListenerVelocity(vx, vy, vz);
}

void CkSound::set3dAttenuation(CkAttenuationMode mode, float nearDist, float farDist, float farVol)
{
    Cki::Sound::set3dAttenuation(mode, nearDist, farDist, farVol);
}

void CkSound::get3dAttenuation(CkAttenuationMode& mode, float& nearDist, float& farDist, float& farVol)
{
    Cki::Sound::get3dAttenuation(mode, nearDist, farDist, farVol);
}

void CkSound::set3dSoundSpeed(float speed)
{
    Cki::Sound::set3dSoundSpeed(speed);
}

float CkSound::get3dSoundSpeed()
{
    return Cki::Sound::get3dSoundSpeed();
}

const float CkSound::k_soundSpeed_CentimetersPerSecond = 34320.0f;
const float CkSound::k_soundSpeed_MetersPerSecond = 343.2f;
const float CkSound::k_soundSpeed_InchesPerSecond = 13512.0f;
const float CkSound::k_soundSpeed_FeetPerSecond = 1126.0f;

CkSound* CkSound::newBankSound(CkBank* bank, int index)
{
    return (CkSound*) Cki::Sound::newBankSound((Cki::Bank*) bank, index);
}

CkSound* CkSound::newBankSound(CkBank* bank, const char* name)
{
    return (CkSound*) Cki::Sound::newBankSound((Cki::Bank*) bank, name);
}

CkSound* CkSound::newStreamSound(const char* filename, CkPathType pathType, int offset, int length, const char* extension)
{
    return (CkSound*) Cki::Sound::newStreamSound(filename, pathType, offset, length, extension);
}

void CkSound::setCustomStreamHandler(CustomStreamFunc func, void* data)
{
    Cki::Sound::setCustomStreamHandler(func, data);
}

#if CK_PLATFORM_IOS
CkSound* CkSound::newAssetStreamSound(const char* url)
{
    return (CkSound*) Cki::Sound::newAssetStreamSound(url);
}
#endif

