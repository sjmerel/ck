#include "ck/audio/audiograph.h"
#include "ck/core/thread.h"
#include "ck/core/logger.h"
#include "ck/core/mem.h"
#include "ck/core/system.h"
#include "ck/core/deletable.h"
#include "ck/core/mutexlock.h"

// template instantiation
#include "ck/core/staticsingleton.cpp"
#include "ck/core/taskqueue.cpp"

namespace Cki
{


void AudioGraph::start()
{
    m_output->start();
}

void AudioGraph::stop()
{
    m_output->stop();
    consumeGraphTasks();
}

void AudioGraph::update()
{
    if (!m_output->isRunning())
    {
        consumeGraphTasks();
    }
}

void AudioGraph::execute(CommandObject* obj, int commandId, CommandParam value0, CommandParam value1)
{
    Task t;
    t.type = k_command;
    t.data[0] = obj;
    t.data[1] = commandId;
    t.data[2] = value0;
    t.data[3] = value1;

    produceGraphTask(t);
}

void AudioGraph::free(void* p)
{
    Task t;
    t.type = k_free;
    t.data[0] = p;

    produceGraphTask(t);
}

void AudioGraph::deleteObject(Deletable* p)
{
    Task t;
    t.type = k_delete;
    t.data[0] = p;

    produceGraphTask(t);
}

MixNode* AudioGraph::getOutputMixer()
{
    return &m_outMix;
}

EffectBus* AudioGraph::getGlobalEffectBus()
{
    return &m_outFx;
}

#if CK_DEBUG
void AudioGraph::print()
{
    get()->m_outMix.print(0);
}
#endif

////////////////////////////////////////

AudioGraph::AudioGraph() :
    m_outFx(true),
    m_graphTasks(System::get()->getConfig().maxAudioTasks)
{

    m_output = GraphOutput::create();
    m_output->setInput(m_outFx.getOutput());
    m_output->setPreRenderCallback(preRenderCallback, this);
    m_output->setPostRenderCallback(postRenderCallback, this);

    m_outFx.getInput()->addInput(&m_outMix);

    start();
}

AudioGraph::~AudioGraph()
{
    // require that it be stopped before dtor
    CK_ASSERT(!m_output->isRunning());
//    stop();

    consumeGraphTasks();

    delete m_output;
}

void AudioGraph::produceGraphTask(const Task& task)
{
    if (m_graphTasks.produce(task))
    {
        CK_LOG_WARNING("Reallocating audio task buffer to size %d; consider increasing CkConfig.maxAudioTasks at startup.", m_graphTasks.getCapacity());
    }
}

void AudioGraph::consumeGraphTasks()
{
    if (m_mutex.tryLock())
    {
        Task task;
        while (m_graphTasks.consume(task))
        {
            switch (task.type)
            {
                case k_command:
                    {
                        CommandObject* obj = (CommandObject*) task.data[0].addrValue;
                        obj->execute(task.data[1].intValue, task.data[2], task.data[3]);
                    }
                    break;

                case k_free:
                    {
                        Mem::free(task.data[0].addrValue);
                    }
                    break;

                case k_delete:
                    {
                        Deletable* d = (Deletable*) task.data[0].addrValue;
                        d->deleteOnUpdate();
                    }
                    break;

                default:
                    CK_FAIL("unknown task type");
            }
        }

        m_mutex.unlock();
    }
    else
    {
        CK_LOG_WARNING("Audio thread could not aquire lock; did you call CkLockAudio() but forget to call CkUnlockAudio()?");
    }
}

void AudioGraph::preRenderCallback(void* data)
{
    AudioGraph* me = (AudioGraph*) data;
    me->preRender();
}

void AudioGraph::preRender()
{
    consumeGraphTasks();
}

void AudioGraph::postRenderCallback(void* data)
{
    AudioGraph* me = (AudioGraph*) data;
    me->postRender();
}

void AudioGraph::postRender()
{
}

void AudioGraph::startCapture(const char* path, CkPathType pathType)
{ 
    m_output->startCapture(path, pathType);
}

void AudioGraph::stopCapture() 
{ 
    m_output->stopCapture(); 
}


template class StaticSingleton<AudioGraph>;

}
