#pragma once

#include "ck/core/platform.h"
#include "ck/core/staticsingleton.h"
#include "ck/core/taskqueue.h"
#include "ck/core/mutex.h"
#include "ck/audio/audionode.h"
#include "ck/audio/graphoutput.h"
#include "ck/audio/mixnode.h"
#include "ck/audio/effectbus.h"

namespace Cki
{

class Deletable;


class AudioGraph : public StaticSingleton<AudioGraph>
{
public:
    void start();
    void stop();
    void update();

    // tasks
    void execute(CommandObject*, int commandId, CommandParam value0 = 0, CommandParam value1 = 0);
    void free(void*);
    void deleteObject(Deletable* p);

    MixNode* getOutputMixer();

    EffectBus* getGlobalEffectBus();

    float getRenderLoad() const { return m_output->getRenderLoad(); }
    float getRenderMs() const { return m_output->getRenderMs(); }
    bool getClipFlag() const { return m_output->getClipFlag(); }
    void resetClipFlag() { return m_output->resetClipFlag(); }

    void lockAudio() { m_mutex.lock(); }
    void unlockAudio() { m_mutex.unlock(); }

    void startCapture(const char* path, CkPathType);
    void stopCapture();

#if CK_DEBUG
    static void print();
#endif

private:
    enum TaskType
    {
        k_command = 1,    // data[0] = CommandObject*; data[1] = int (id); command data follows
        k_free,           // data[0] = pointer
        k_delete          // data[0] = pointer
    };

    struct Task
    {
        TaskType type;
        CommandParam data[4];
    };

    MixNode m_outMix; // final mixer
    EffectBus m_outFx; // final effects
    GraphOutput* m_output;
    Mutex m_mutex; // for CkLock()/CkUnlock()

    TaskQueue<Task> m_graphTasks; // tasks for the graph thread

    void produceGraphTask(const Task&);
    void consumeGraphTasks();

    static void preRenderCallback(void* data);
    void preRender();
    static void postRenderCallback(void* data);
    void postRender();

    friend class StaticSingleton<AudioGraph>;
    AudioGraph();
    ~AudioGraph();

    AudioGraph(const AudioGraph&);
    AudioGraph& operator=(const AudioGraph&);
};

// silence warning
extern template class StaticSingleton<AudioGraph>;

}
