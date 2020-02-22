#include "ck/audio/mixer.h"
#include "ck/core/debug.h"
#include "ck/core/logger.h"
#include <new>

// template instantiations
#include "ck/core/treenode.cpp"
#include "ck/core/proxied.cpp"

namespace Cki
{



Mixer* Mixer::s_master = NULL;
char Mixer::s_masterMem[sizeof(Mixer)];

Mixer::Mixer() :
    m_name("master"),
    m_localVol(1.0f),
    m_mixVol(1.0f),
    m_localPaused(false),
    m_mixPaused(false)
{
}

Mixer::Mixer(const char* name, Mixer* parent) :
    m_name(name),
    m_localVol(1.0f),
    m_mixVol(1.0f),
    m_localPaused(false),
    m_mixPaused(false)
{
    if (this != s_master)
    {
        setParent(parent ? parent : s_master);
    }
}

Mixer::~Mixer()
{
    Mixer* parent = getParent();

    // replace all sounds' mixers with parent, or null
    Sound* sound = m_sounds.getFirst();
    while (sound)
    {
        sound->setMixer(parent);
        sound = ((SoundMixerList::Node*) sound)->getNext();
    }

    if (parent)
    {
        remove();

        // parent children to my parent
        Mixer* child;
        while ((child = getFirstChild()))
        {
            parent->addChild(child);
            child->update(); // recalculate volumes since hierarchy has changed
        }
    }
}

void Mixer::destroy()
{
    if (getMaster() == this)
    {
        CK_LOG_ERROR("Can't destroy the master mixer!");
    }
    else
    {
        delete this;
    }
}

void Mixer::setName(const char* name)
{
    m_name.set(name);
}

const char* Mixer::getName() const
{
    return m_name.getBuffer();
}

void Mixer::setVolume(float vol)
{
    if (vol != m_localVol)
    {
        m_localVol = vol;
        update();
    }
}

float Mixer::getVolume() const
{
    return m_localVol;
}

float Mixer::getMixedVolume() const
{
    return m_mixVol;
}

void Mixer::setPaused(bool paused)
{
    if (paused != m_localPaused)
    {
        m_localPaused = paused;
        update();
    }
}

bool Mixer::isPaused() const
{
    return m_localPaused;
}

bool Mixer::getMixedPauseState() const
{
    return m_mixPaused;
}

void Mixer::setParent(CkMixer* parent)
{
    if (this == s_master)
    {
        CK_LOG_ERROR("Master mixer cannot have a parent");
        return;
    }
    if (this == parent)
    {
        CK_LOG_ERROR("Cannot parent a mixer to itself");
        return;
    }

    if (!parent)
    {
        parent = s_master;
    }

    ((Mixer*) parent)->addChild(this);
    update();
}

Mixer* Mixer::getParent()
{
    return TreeNode<Mixer>::getParent();
}

void Mixer::addSound(Sound* sound)
{
    m_sounds.addFirst(sound);
}

void Mixer::removeSound(Sound* sound)
{
    m_sounds.remove(sound);
}

Mixer* Mixer::getMaster()
{
    return s_master;
}

Mixer* Mixer::newMixer(const char* name, Mixer* parent)
{
    return new Mixer(name, parent);
}

Mixer* Mixer::find(const char* name)
{
    // TODO use hashtable to speed up this search
    Mixer* p = s_master;

    while (p)
    {
        if (p->m_name == name)
        {
            return p;
        }

        p = p->getNext();
    }

    if (!p)
    {
        CK_LOG_INFO("Couldn't find mixer named \"%s\"", name);
        return NULL;
    }

    return p;
}

////////////////////////////////////////

void Mixer::update()
{
    float mixVol;
    bool mixPaused = false;
    Mixer* parent = getParent();
    if (parent)
    {
        mixVol = m_localVol * parent->m_mixVol;
        mixPaused = m_localPaused || parent->m_mixPaused;
    }
    else
    {
        CK_ASSERT(this == s_master);
        mixVol = m_localVol;
        mixPaused = m_localPaused;
    }

    bool updateChildren = false;

    if (mixVol != m_mixVol)
    {
        m_mixVol = mixVol;
        updateChildren = true;

        // update child sounds
        Sound* sound = m_sounds.getFirst();
        while (sound)
        {
            sound->updateVolumeAndPan();
            sound = ((SoundMixerList::Node*) sound)->getNext();
        }
    }

    if (mixPaused != m_mixPaused)
    {
        m_mixPaused = mixPaused;
        updateChildren = true;

        // update child sounds
        Sound* sound = m_sounds.getFirst();
        while (sound)
        {
            sound->updatePaused();
            sound = ((SoundMixerList::Node*) sound)->getNext();
        }
    }

    if (updateChildren)
    {
        // update child mixers
        Mixer* child = getFirstChild();
        while (child)
        {
            child->update();
            child = child->getNextSibling();
        }
    }
}

#if CK_DEBUG
void Mixer::debug(int level)
{
    for (int i = 0; i < level; ++i)
    {
        CK_PRINT("  ");
    }
    CK_PRINT("%.15s %f %f %d %d\n", m_name.getBuffer(), m_localVol, m_mixVol, m_localPaused, m_mixPaused);

    Mixer* child = getFirstChild();
    while (child)
    {
        CK_ASSERT(child->getParent() == this);
        child->debug(level + 1);
        child = child->getNextSibling();
    }
}
#endif

void Mixer::init()
{
    s_master = new (s_masterMem) Mixer();
}

void Mixer::shutdown()
{
    s_master->~Mixer();
    s_master = NULL;
}

template class Proxied<Mixer>;
template class TreeNode<Mixer>;

}
