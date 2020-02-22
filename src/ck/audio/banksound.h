#pragma once

#include "ck/core/platform.h"
#include "ck/core/list.h"
#include "ck/audio/buffersource.h"
#include "ck/audio/graphsound.h"

namespace Cki
{

struct Sample;
class Bank;

class BankSound : 
    public GraphSound,
    public List<BankSound>::Node
{
public:
    BankSound(const Sample&, Bank*);

    virtual void destroy();

    virtual void play();

    virtual void setPlayPosition(int);
    virtual void setPlayPositionMs(float);

    void unload();

private:
    BufferSource m_source;
    Bank* m_bank;
};


}

