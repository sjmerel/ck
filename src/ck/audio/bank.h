#pragma once

#include "ck/bank.h"
#include "ck/core/platform.h"
#include "ck/core/listable.h"
#include "ck/core/proxied.h"
#include "ck/core/allocatable.h"
#include "ck/core/deletable.h"
#include "ck/audio/bankdata.h"
#include "ck/audio/banksound.h"
#include "ck/pathtype.h"

namespace Cki
{

class BinaryStream;

class Bank : 
    public CkBank,
    public Allocatable, 
    public Deletable, 
    public Listable<Bank>, 
    public Proxied<Bank>
{
public:
    void destroy();

    bool isLoaded() const;
    bool isFailed() const;

    const char* getName() const;
    int getNumSounds() const;
    const char* getSoundName(int index) const;

    const Sample& getSample(int index) const;
    const Sample* findSample(const char*) const;

    static Bank* find(const char* name);
    static const Sample* findSample(const char* name, Bank** bank);

    static Bank* newBank(const char* path, CkPathType, int offset, int length);
    static Bank* newBankAsync(const char* path, CkPathType, int offset, int length);
    static Bank* newBankFromMemory(void*, int bytes);

    void addSound(BankSound*);
    void removeSound(BankSound*);

    static void destroyAll();

    static void setCreateCallback(Callback1<Bank*>::Func, void*);
    static void setDestroyCallback(Callback1<Bank*>::Func, void*);

private:
    BankData* m_data;
    void* m_buf; // NULL if memory is managed externally
    int m_size;
    bool m_loaded;
    List<BankSound> m_sounds;

    Bank(BankData* data, void* buf, int size);
    Bank(void* buf, int offset, int size, const char* path, CkPathType);

    static BankData* process(void* buf, int size);

    static void asyncLoadCallback(bool result, void* data);
    void onAsyncLoad(bool result);

    static Bank* create(void* buf, int size, bool external);
    static Bank* create(BinaryStream&, int offset, int length);

};


}
