#include "ck/core/math.h"
#include "ck/audio/volumematrix.h"

namespace Cki
{

void VolumeMatrix::setPan(float pan)
{
    pan = Math::clamp(pan, -1.0f, 1.0f);

#if 0
    ll = Math::sqrt((1.0f - pan) * 0.5f);
    rr = Math::sqrt((1.0f + pan) * 0.5f);
#else
    // this follows MIDI recommended practice:
    // http://www.midi.org/techspecs/rp36.php
    float x = Math::k_pi_4*(pan + 1.0f);
    ll = Math::cos(x); // [1..0]
    rr = Math::sin(x); // [0..1]
#endif
    rl = lr = 0.0f;
}

float VolumeMatrix::getPan() const
{
    // Matrix volumes were not necessarily set with setPan()!
    // Ignore lr and rl components. 

    // normalize so that rr*rr + ll*ll = 1
    float power = rr*rr + ll*ll;
    if (power < 0.0001f)
    {
        return 0.0f;
    }
    float rNorm = Math::abs(rr) / Math::sqrt(power);

    return (Math::asin(rNorm) / Math::k_pi_4) - 1.0f;
}

/*
void VolumeMatrix::setVolumePan(float volume, float pan)
{
    pan = Math::clamp(pan, -1.0f, 1.0f);

    // this follows MIDI recommended practice:
    // http://www.midi.org/techspecs/rp36.php
    float x = Math::k_pi_4*(pan + 1.0f);
    ll = volume * Math::cos(x); // [1..0]
    rr = volume * Math::sin(x); // [0..1]
    rl = lr = 0.0f;
}

void VolumeMatrix::getVolumePan(float& volume, float& pan) const
{
    // Matrix volumes were not necessarily set with setPan()!
    // Ignore lr and rl components. 
    
    // normalize so that rr*rr + ll*ll = 1
    float power = rr*rr + ll*ll;
    volume = Math::sqrt(power);
    if (power < 0.0001f)
    {
        pan = 0.0f;
    }
    else
    {
        float rNorm = Math::abs(rr) / volume;
        pan = (Math::asin(rNorm) / Math::k_pi_4) - 1.0f;
    }
}
*/

void VolumeMatrix::setStereoPan(float pan)
{
    pan = Math::clamp(pan, -1.0f, 1.0f);

#if 0
    if (pan < 0.0f)
    {
        ll = 1.0f;                     // [1..1]
        lr = Math::sqrt(-pan);         // [1..0]
        rl = 0.0f;                     // [0..0]
        rr = Math::sqrt(pan + 1.0f);   // [0..1]
    }
    else
    {
        ll = Math::sqrt(1.0f - pan);
        lr = 0.0f;
        rl = Math::sqrt(pan);
        rr = 1.0f;
    }
#else
    float x = Math::k_pi_2 * pan;
    if (pan < 0.0f)
    {
        ll = 1.0f;          // [1..1]
        lr = -Math::sin(x); // [1..0]
        rl = 0.0f;          // [0..0]
        rr = Math::cos(x);  // [0..1]
    }
    else
    {
        ll = Math::cos(x);
        lr = 0.0f;
        rl = Math::sin(x);
        rr = 1.0f;
    }
#endif

    // scale so power is 0.5; 
    // otherwise panning a full-scale input can cause clipping.
    *this *= 0.5f;
}

float VolumeMatrix::getStereoPan() const
{
    // Matrix volumes were not necessarily set with setStereoPan()!

    // normalize so that power = 0.5
    float llSq = ll*ll;
    float lrSq = lr*lr;
    float rlSq = rl*rl;
    float rrSq = rr*rr;
    float power = llSq + lrSq + rlSq + rrSq;
    if (power < 0.0001f)
    {
        return 0.0f;
    }

    float scaleSq = 0.5f / power;

    float rrNormSq = rrSq * scaleSq;
    float rlNormSq = rlSq * scaleSq;

    // power of right channel 
    float rightPower = rrNormSq + rlNormSq;
    if (rightPower > 0.25f)
    {
        return Math::asin(Math::sqrt(4*rightPower - 1.0f)) / Math::k_pi_2;
    }
    else
    {
        return -Math::acos(Math::sqrt(4*rightPower)) / Math::k_pi_2;
    }
}

/*
void VolumeMatrix::setStereoVolumePan(float volume, float pan)
{
    pan = Math::clamp(pan, -1.0f, 1.0f);

    // scale so power is 0.5; otherwise panning a full-scale input can cause clipping.
    float s = 0.5f * volume; 

    float x = Math::k_pi_2 * pan;
    if (pan < 0.0f)
    {
        ll = s;                 // [s..s]
        lr = -s * Math::sin(x); // [s..0]
        rl = 0.0f;              // [0..0]
        rr = s * Math::cos(x);  // [0..s]
    }
    else
    {
        ll = s * Math::cos(x);
        lr = 0.0f;
        rl = s * Math::sin(x);
        rr = s;
    }
}

void VolumeMatrix::getStereoVolumePan(float& volume, float& pan) const
{
    // Matrix volumes were not necessarily set with setStereoPan()!

    // normalize so that power = 0.5
    float llSq = ll*ll;
    float lrSq = lr*lr;
    float rlSq = rl*rl;
    float rrSq = rr*rr;
    float power = llSq + lrSq + rlSq + rrSq;

    volume = Math::sqrt(2.0f*power);

    if (power < 0.0001f)
    {
        pan = 0.0f;
    }
    else
    {
        float scaleSq = 0.5f / power;

        float rrNormSq = rrSq * scaleSq;
        float rlNormSq = rlSq * scaleSq;

        // power of right channel 
        float rightPower = rrNormSq + rlNormSq;
        if (rightPower > 0.25f)
        {
            pan = Math::asin(Math::sqrt(4*rightPower - 1.0f)) / Math::k_pi_2;
        }
        else
        {
            pan = -Math::acos(Math::sqrt(4*rightPower)) / Math::k_pi_2;
        }
    }
}
*/

float VolumeMatrix::getPower() const
{
    return rr*rr + lr*lr + rl*rl + ll*ll;
}


}
