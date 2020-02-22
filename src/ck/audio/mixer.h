#pragma once

#include "ck/mixer.h"
#include "ck/core/platform.h"
#include "ck/core/proxied.h"
#include "ck/core/treenode.h"
#include "ck/core/list.h"
#include "ck/audio/soundname.h"
#include "ck/audio/sound.h"

namespace Cki
{


class Mixer : 
    public CkMixer,
    public Allocatable, 
    private TreeNode<Mixer>, 
    public Proxied<Mixer>
{
public:
    Mixer(const char* name, Mixer* parent = NULL);
    virtual ~Mixer();

    void destroy();

    void setName(const char*);
    const char* getName() const;

    // local volume
    void setVolume(float);
    float getVolume() const;

    // mixed volume (affected by parents)
    float getMixedVolume() const;

    void setPaused(bool);
    bool isPaused() const;
    bool getMixedPauseState() const;

    // setting parent to NULL sets the master mixer as parent
    void setParent(CkMixer*);
    Mixer* getParent();

    static Mixer* getMaster();
    static Mixer* newMixer(const char* name, Mixer* parent = NULL);
    static Mixer* find(const char* name);

#if CK_DEBUG
    void debug(int level = 0);
#endif

    static void init();
    static void shutdown();

    // should be called only by Sound:
    void addSound(Sound*);
    void removeSound(Sound*);

private:
    SoundName m_name;
    float m_localVol;
    float m_mixVol;
    bool m_localPaused;
    bool m_mixPaused;
    SoundMixerList m_sounds;

    static Mixer* s_master;
    static char s_masterMem[];

    Mixer(); // master only
    void update();
};


}
