#include "ck/ck.h"
#include "ck/config.h"
#include "ck/bank.h"
#include "ck/sound.h"
#include <jni.h>
#include <unistd.h>

extern "C"
{

void 
Java_com_crickettechnology_samples_HelloCricketNDK_MainActivity_helloCricketNative(JNIEnv* env, jobject activity)
{
    CkConfig config(env, activity);
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

    CkShutdown();
}


}

