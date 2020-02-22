#include "ck/core/bufferstream.h"
#include "ck/core/swapstream.h"
#include "ck/core/readstream.h"
#include "ck/core/mem.h"
#include "ck/core/asyncloader.h"
#include "ck/core/debug.h"
#include "ck/core/memoryfixup.h"
#include "ck/core/logger.h"
#include "ck/core/path.h"
#include "ck/core/thread.h"
#include "ck/audio/fileheader.h"
#include "ck/audio/audiograph.h"
#include "ck/audio/bank.h"

#include "ck/core/listable.cpp" // template definition
#include "ck/core/proxied.cpp" // template definition

namespace Cki
{


void Bank::destroy()
{
    if (!m_loaded)
    {
        // TODO better way to deal with this?
        CK_LOG_WARNING("Bank is being destroyed before it is loaded; waiting for it to finish");
        while (!m_loaded)
        {
            Thread::sleepMs(10);
        }
    }

    // remove from list
    Listable<Bank>::destroy();

    // unload all sounds
    BankSound* sound = NULL;
    while ((sound = m_sounds.getFirst()))
    {
        sound->unload();
        m_sounds.remove(sound);
    }

    AudioGraph::get()->deleteObject(this);
    AudioGraph::get()->free(m_buf);
}

bool Bank::isLoaded() const
{
    return m_loaded;
}

bool Bank::isFailed() const
{
    return m_loaded && !m_data;
}

const char* Bank::getName() const
{
    if (!m_data)
    {
        CK_LOG_ERROR("Bank has not been loaded");
        return NULL;
    }

    return m_data->getName().getBuffer();
}

int Bank::getNumSounds() const
{
    if (!m_data)
    {
        CK_LOG_ERROR("Bank has not been loaded");
        return 0;
    }

    return m_data->getNumSamples();
}

const char* Bank::getSoundName(int index) const
{
    if (!m_data)
    {
        CK_LOG_ERROR("Bank has not been loaded");
        return NULL;
    }

    if (index < 0 || index >= m_data->getNumSamples())
    {
        CK_LOG_ERROR("Index %d out of range; bank \"%s\" has only %d sounds",
                index, getName(), getNumSounds());
        return NULL;
    }

    return m_data->getSample(index).name.getBuffer();
}

const Sample& Bank::getSample(int index) const
{
    CK_ASSERT(m_loaded);
    CK_ASSERT(!isFailed());
    return m_data->getSample(index);
}

const Sample* Bank::findSample(const char* name) const
{
    CK_ASSERT(m_loaded);
    CK_ASSERT(!isFailed());
    return m_data->findSample(name);
}

Bank* Bank::find(const char* name)
{
    // TODO use hashtable to speed up this search
    Bank* bank = Listable<Bank>::getFirst();
    while (bank)
    {
        if (bank->isLoaded() && !bank->isFailed())
        {
            if (bank->m_data->getName() == name)
            {
                return bank;
            }
        }
        bank = ((List<Bank>::Node*) bank)->getNext();
    }
    CK_LOG_INFO("Could not find bank named \"%s\"", name);
    return NULL;
}

const Sample* Bank::findSample(const char* name, Bank** bankOut)
{
    Bank* bank = Listable<Bank>::getFirst();
    while (bank)
    {
        if (bank->isLoaded() && !bank->isFailed())
        {
            const Sample* sample = bank->findSample(name);
            if (sample)
            {
                if (bankOut)
                {
                    *bankOut = bank;
                }
                return sample;
            }
        }
        bank = ((List<Bank>::Node*) bank)->getNext();
    }
    if (bankOut)
    {
        *bankOut = NULL;
    }
    return NULL;
}

Bank* Bank::newBank(const char* path, CkPathType pathType, int offset, int length)
{
    ReadStream file(path, pathType);
    if (file.isValid())
    {
        int fileSize = file.getSize();
        if (offset < 0 || offset >= fileSize)
        {
            CK_LOG_ERROR("Invalid offset %d for bank file \"%s\"", offset, Path(path, pathType).getBuffer());
            return NULL;
        }
        if (length <= 0)
        {
            length = fileSize - offset;
        }
        if (length > fileSize - offset)
        {
            CK_LOG_ERROR("Invalid length %d for bank file \"%s\"", length, Path(path, pathType).getBuffer());
            return NULL;
        }

        Bank* bank = Bank::create(file, offset, length);
        if (!bank)
        {
            CK_LOG_ERROR("Bank from file \"%s\" could not be processed", Path(path, pathType).getBuffer());
        }
        return bank;
    }
    else
    {
        Path fullPath(path, pathType);
        CK_LOG_ERROR("Bank file \"%s\" could not be opened", fullPath.getBuffer());
        return NULL;
    }
}

Bank* Bank::newBankAsync(const char* path, CkPathType pathType, int offset, int length)
{
    int fileSize = ReadStream::getSize(path, pathType);
    if (fileSize >= 0)
    {
        if (offset < 0 || offset >= fileSize)
        {
            CK_LOG_ERROR("Invalid offset %d for bank file \"%s\"", offset, Path(path, pathType).getBuffer());
            return NULL;
        }
        if (length <= 0)
        {
            length = fileSize - offset;
        }
        if (length > fileSize - offset)
        {
            CK_LOG_ERROR("Invalid length %d for bank file \"%s\"", length, Path(path, pathType).getBuffer());
            return NULL;
        }

        void* buf = Mem::alloc(length);
        if (!buf)
        {
            return NULL;
        }
        Bank* bank = new Bank(buf, offset, length, path, pathType);
        if (!bank)
        {
            Mem::free(buf);
        }
        return bank;
    }
    else
    {
        Path fullPath(path, pathType);
        CK_LOG_ERROR("Bank file \"%s\" could not be opened", fullPath.getBuffer());
        return NULL;
    }
}

Bank* Bank::newBankFromMemory(void* buf, int bytes)
{
    Bank* bank = Bank::create(buf, bytes, true);
    if (!bank)
    {
        CK_LOG_ERROR("Bank from memory location 0x%p could not be processed", buf);
    }
    return bank;
}

void Bank::addSound(BankSound* sound)
{
    m_sounds.addFirst(sound);
}

void Bank::removeSound(BankSound* sound)
{
    m_sounds.remove(sound);
}

void Bank::destroyAll()
{
    Bank* bank = Listable<Bank>::getFirst();
    while (bank)
    {
        Bank* prevBank = bank;
        bank = ((List<Bank>::Node*) bank)->getNext();
        prevBank->destroy();
    }
}

////////////////////////////////////////

Bank::Bank(BankData* data, void* buf, int size) :
    m_data(data),
    m_buf(buf),
    m_size(size),
    m_loaded(true)
{
}

Bank::Bank(void* buf, int offset, int size, const char* path, CkPathType pathType) :
    m_data(NULL),
    m_buf(buf),
    m_size(size),
    m_loaded(false)
{
    bool loading = AsyncLoader::get()->load(path, pathType, buf, offset, size, asyncLoadCallback, this);
    if (!loading)
    {
        m_loaded = true;
    }
}

void Bank::asyncLoadCallback(bool result, void* data)
{
    Bank* bank = (Bank*) data;
    bank->onAsyncLoad(result);
}

void Bank::onAsyncLoad(bool result)
{
    CK_ASSERT(m_buf);
    CK_ASSERT(!m_data);
    if (result)
    {
        m_data = process(m_buf, m_size);
    }
    else
    {
        CK_LOG_ERROR("Bank async load failed");
    }

#if CK_PLATFORM_WIN
    MemoryBarrier();
#else
    __sync_synchronize();
#endif
    m_loaded = true;
}

BankData* Bank::process(void* buf, int size)
{
    BufferStream bs(buf, size, size);
    SwapStream swapper(bs);

    FileHeader fileHeader;
    swapper >> fileHeader;

    if ((fileHeader.marker != FileHeader::k_marker))
    {
        CK_LOG_ERROR("Invalid file marker: \"%.4s\" (expecting \"%.4s\")", fileHeader.marker.getData(), FileHeader::k_marker.getData());
        return NULL;
    }
    if (!(fileHeader.targets & Target::k_current))
    {
        CK_LOG_ERROR("Invalid file target mask: %x (does not include current target %x)", fileHeader.targets, Target::k_current);
        return NULL;
    }
    if (fileHeader.fileType != FileType::k_bank)
    {
        CK_LOG_ERROR("Invalid file type: %d (expecting %d)", fileHeader.fileType, FileType::k_bank);
        return NULL;
    }
    if (fileHeader.fileVersion != FileHeader::k_version)
    {
        CK_LOG_ERROR("Invalid file version: %d (expecting %d)", fileHeader.fileVersion, FileHeader::k_version);
        if (fileHeader.fileVersion > FileHeader::k_version)
        {
            CK_LOG_ERROR(" Are you linking to an older version of the Cricket library?");
        }
        else
        {
            CK_LOG_ERROR(" You may need to rebuild your banks with the latest version of cktool.");
        }
        return NULL;
    }

    byte* bankBuf = (byte*) buf + bs.getPos();
    BankData* bankData = (BankData*) (bankBuf);
    MemoryFixup fixup(bankBuf, bs.getSize() - bs.getPos());
    fixup.fixup(*bankData);
    if (fixup.isFailed())
    {
        CK_LOG_ERROR("Corrupted or truncated bank data");
        return NULL;
    }
    else
    {
        return bankData;
    }
}

Bank* Bank::create(void* buf, int size, bool external)
{
    BankData* bankData = process(buf, size);
    if (bankData)
    {
        if (external)
        {
            return new Bank(bankData, NULL, 0);
        }
        else
        {
            return new Bank(bankData, buf, size);
        }
    }
    else
    {
        return NULL;
    }
}

Bank* Bank::create(BinaryStream& stream, int offset, int length)
{
    stream.setPos(offset);
    void* buf = Mem::alloc(length);
    if (!buf)
    {
        return NULL;
    }
    int bytesRead = stream.read(buf, length);

    if (bytesRead == length)
    {
        Bank* bank = create(buf, length, false);
        if (bank)
        {
            return bank;
        }
    }

    Mem::free(buf);
    return NULL;
}


template class Proxied<Bank>;

}
