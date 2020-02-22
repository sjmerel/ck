#include "bank.h"
#include "proxy.h"
#include "stringconvert.h"
#include "ck/bank.h"

using namespace Platform;


namespace CricketTechnology
{
namespace Audio
{


bool Bank::Loaded::get()
{
    return m_impl->isLoaded();
}

bool Bank::Failed::get()
{
    return m_impl->isFailed();
}

String^ Bank::Name::get()
{
    return StringConvert<256>::newPlatformString(m_impl->getName());
}

int Bank::NumSounds::get()
{
    return m_impl->getNumSounds();
}

String^ Bank::GetSoundName(int index)
{
    return StringConvert<256>::newPlatformString(m_impl->getSoundName(index));
}

Bank^ Bank::NewBank(String^ path, PathType pathType, int offset, int length)
{
    StringConvert<256> convert(path);
    CkBank* impl = CkBank::newBank(convert.getCString(), (CkPathType) pathType, offset, length);
    return Proxy::GetBank(impl);
}

Bank^ Bank::NewBank(String^ path, PathType pathType)
{
    return NewBank(path, pathType, 0, 0);
}

Bank^ Bank::NewBank(String^ path)
{
    return NewBank(path, PathType::Default, 0, 0);
}

Bank^ Bank::NewBankAsync(String^ path, PathType pathType, int offset, int length)
{
    StringConvert<256> convert(path);
    CkBank* impl = CkBank::newBankAsync(convert.getCString(), (CkPathType) pathType, offset, length);
    return Proxy::GetBank(impl);
}

Bank^ Bank::NewBankAsync(String^ path, PathType pathType)
{
    return NewBankAsync(path, pathType, 0, 0);
}

Bank^ Bank::NewBankAsync(String^ path)
{
    return NewBankAsync(path, PathType::Default, 0, 0);
}

Bank^ Bank::Find(String^ bankName)
{
    StringConvert<256> convert(bankName);
    CkBank* impl = CkBank::find(convert.getCString());
    return Proxy::GetBank(impl);
}

void Bank::Destroy()
{
    if (m_impl)
    {
        CkBank* impl = m_impl;
        m_impl = NULL;
        impl->destroy();
    }
}

Bank::Bank(CkBank* impl) :
    m_impl(impl)
{}

Bank::~Bank()
{
    Destroy();
}

}
}
