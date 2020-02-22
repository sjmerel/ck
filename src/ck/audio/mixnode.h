#pragma once

#include "ck/core/platform.h"
#include "ck/core/timer.h"
#include "ck/audio/audionode.h"

namespace Cki
{


class MixNode : public AudioNode
{
public:
    MixNode();
    virtual ~MixNode();

    virtual BufferHandle processFloat(int frames, bool post, bool& needsPost);
    virtual BufferHandle processFixed(int frames, bool post, bool& needsPost);

    enum
    {
        k_addInput,
        k_removeInput,
        k_detach
    };
    virtual void execute(int cmdId, CommandParam param0, CommandParam param1);

    void addInput(AudioNode*);
    void removeInput(AudioNode*);

#if CK_DEBUG
    virtual void print(int level);
#endif

private:
    List<AudioNode> m_inputs;
    void* m_bufMem;
    SharedBuffer m_buf;

    template <typename T> BufferHandle process(int frames, bool post, bool& needsPost);
    void allocateBuffer();
    static Timer s_cpuWarningTimer;
    static void printCpuWarning(float ms);
};


}
