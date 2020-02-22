#pragma once
#include "ck/effectprocessor.h"

// Custom effect processor, implementing a simple "stutter" effect,
// in which the audio alternates between normal volume and silence.

class StutterProcessor : public CkEffectProcessor
{
public:
    StutterProcessor();

    virtual ~StutterProcessor() {}

    // This effect takes one parameter, the period of the effect in milliseconds.
    // This constant defines the parameter ID for that parameter.
    enum { k_periodMs = 0 };

    virtual void setParam(int paramId, float value);

    virtual void reset();

    virtual bool isInPlace() const;

    // This method is for the fixed-point pipeline, used only by older Android devices;
    // it will be removed at some point in the future when enough Android devices have
    // hardware floating-point support. If you are not supporting Android, or don't care
    // about compatibility with older low-end devices, you may leave this with its
    // default implementation, which passes the audio through unchanged.
    virtual void process(int* inBuf, int* outBuf, int frames);

    virtual void process(float* inBuf, float* outBuf, int frames);

    // This is a factory function which is passed to CkEffect::registerCustomEffect().
    static CkEffectProcessor* create(void*);

private:
    int m_periodFrames; // number of frames in one period
    int m_frames; // number of frames elapsed in the current period
};

