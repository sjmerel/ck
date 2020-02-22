#include "ck/audio/banksound.h"
#include "ck/audio/bank.h"
#include "ck/audio/sample.h"
#include "ck/audio/audiograph.h"
#include "ck/core/debug.h"
#include "ck/core/logger.h"

namespace Cki
{


BankSound::BankSound(const Sample& sample, Bank* bank) :
    m_source(sample),
    m_bank(bank)
{
    subInit(&m_source);
    bank->addSound(this);
}

void BankSound::destroy()
{
    GraphSound::destroy();
    if (m_bank)
    {
        m_bank->removeSound(this);
    }
}

void BankSound::unload()
{
    m_bank = NULL;
    if (isPlaying())
    {
        CK_LOG_ERROR("Stopping sound; bank has been unloaded");
    }
    stop();
    removePrevSounds();
}

void BankSound::play()
{
    if (!m_bank)
    {
        CK_LOG_ERROR("Could not play sound; bank has been unloaded");
    }
    else
    {
        GraphSound::play();
    }
}

void BankSound::setPlayPosition(int frame)
{
    AudioGraph::get()->execute(&m_sourceNode, SourceNode::k_setPlayPosition, frame);
}

void BankSound::setPlayPositionMs(float ms)
{
    AudioGraph::get()->execute(&m_sourceNode, SourceNode::k_setPlayPositionMs, ms);
}


}
