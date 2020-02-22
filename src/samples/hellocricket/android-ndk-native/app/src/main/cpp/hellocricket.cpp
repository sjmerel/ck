#include <android_native_app_glue.h>
#include <android/log.h>
#include "ck/ck.h"
#include "ck/sound.h"
#include "ck/bank.h"
#include <unistd.h>


extern "C"
void android_main(struct android_app* state)
{
    // The android_native_app_glue library runs android_main from a separate native
    // thread which has not yet been attached to the JVM.  Therefore the JNIEnv pointer
    // in state->activity->env cannot be used from this thread!  You must use the
    // CkConfig constructor that takes a JavaVM* parameter, not the one that takes a
    // JNIEnv* paramter.
    CkConfig config(state->activity->vm, state->activity->clazz);
    CkInit(&config);

    CkBank* bank = CkBank::newBank("hellocricket.ckb");
    CkSound* sound = CkSound::newBankSound(bank, 0);
    sound->play();

    while (sound->isPlaying())
    {
        CkUpdate();
        usleep(30000);
    }

    sound->destroy();
    bank->destroy();

    // main event loop (which doesn't really do anything here)
    while (true)
    {
        int ident;
        int events;
        struct android_poll_source* source;

        while ((ident=ALooper_pollAll(-1, NULL, &events, (void**)&source)) >= 0) 
        {
            if (source)
            {
                source->process(state, source);
            }

            if (state->destroyRequested != 0) 
            {
                break;
            }
        }
    }

    CkShutdown();
}

