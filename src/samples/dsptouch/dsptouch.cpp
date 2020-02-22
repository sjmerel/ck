#include "dsptouch.h"
#include "stutterprocessor.h"
#include "ck/ck.h"
#include "ck/bank.h"
#include "ck/sound.h"
#include "ck/effect.h"
#include "ck/effectparam.h"
#include "ck/effectbus.h"

#include <stddef.h>


#define EFFECT_TYPE_BITCRUSHER 0
#define EFFECT_TYPE_RINGMOD 1
#define EFFECT_TYPE_DISTORTION 2
#define EFFECT_TYPE_STUTTER 3

// set EFFECT_TYPE to determine which effect we use:
#define EFFECT_TYPE EFFECT_TYPE_BITCRUSHER


namespace
{
    CkBank* g_bank = NULL;
    CkSound* g_sound = NULL;
    CkEffect* g_effect = NULL;
}

#ifdef CK_PLATFORM_ANDROID
void dspTouchInit(JNIEnv* env, jobject activity)
#else
void dspTouchInit()
#endif
{
#ifdef CK_PLATFORM_ANDROID
    CkConfig cfg(env, activity);
#else
    CkConfig cfg;
#endif
    CkInit(&cfg);

    g_bank = CkBank::newBank("dsptouch.ckb");
    g_sound = CkSound::newBankSound(g_bank, "hiphoppiano");
    g_sound->setLoopCount(-1);
    g_sound->play();

#if EFFECT_TYPE == EFFECT_TYPE_BITCRUSHER
    g_effect = CkEffect::newEffect(kCkEffectType_BitCrusher);
#elif EFFECT_TYPE == EFFECT_TYPE_RINGMOD
    g_effect = CkEffect::newEffect(kCkEffectType_RingMod);
#elif EFFECT_TYPE == EFFECT_TYPE_DISTORTION
    g_effect = CkEffect::newEffect(kCkEffectType_Distortion);
#elif EFFECT_TYPE == EFFECT_TYPE_STUTTER
    enum { k_stutterEffectId = 0 };
    CkEffect::registerCustomEffect(k_stutterEffectId, StutterProcessor::create);
    g_effect = CkEffect::newCustomEffect(k_stutterEffectId);
#else
#  error "unknown effect type"
#endif

    g_effect->setBypassed(true);
    CkEffectBus::getGlobalEffectBus()->addEffect(g_effect);
}

void dspTouchUpdate()
{
    CkUpdate();
}

void dspTouchShutdown()
{
    g_sound->destroy();
    g_bank->destroy();
    g_effect->destroy();

    CkShutdown();
}

void dspTouchSuspend()
{
    CkSuspend();
}

void dspTouchResume()
{
    CkResume();
}

void dspTouchStartEffect()
{
    g_effect->setBypassed(false);
}

void dspTouchSetEffectParams(float x, float y)
{
#if EFFECT_TYPE == EFFECT_TYPE_BITCRUSHER
    g_effect->setParam(kCkBitCrusherParam_BitResolution, x*25.0f);
    g_effect->setParam(kCkBitCrusherParam_HoldMs, y*2.0f);
#elif EFFECT_TYPE == EFFECT_TYPE_RINGMOD
    g_effect->setParam(kCkRingModParam_Freq, x*1000.0f);
#elif EFFECT_TYPE == EFFECT_TYPE_DISTORTION
    g_effect->setParam(kCkDistortionParam_Drive, x*10.0f);
    g_effect->setParam(kCkDistortionParam_Offset, y);
#elif EFFECT_TYPE == EFFECT_TYPE_STUTTER
    g_effect->setParam(StutterProcessor::k_periodMs, 50.0f + x*150.0f);
#endif
}

void dspTouchStopEffect()
{
    g_effect->setBypassed(true);
}
