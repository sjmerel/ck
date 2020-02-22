#include "ck/bank.h"
#include "ck/audio/bank.h"


CkBank* CkBank::newBank(const char* filename, CkPathType pathType, int offset, int length)
{
    return (CkBank*) Cki::Bank::newBank(filename, pathType, offset, length);
}

CkBank* CkBank::newBankAsync(const char* filename, CkPathType pathType, int offset, int length)
{
    return (CkBank*) Cki::Bank::newBankAsync(filename, pathType, offset, length);
}

CkBank* CkBank::newBankFromMemory(void* buf, int bytes)
{
    return (CkBank*) Cki::Bank::newBankFromMemory(buf, bytes);
}

CkBank* CkBank::find(const char* bankName)
{
    return (CkBank*) Cki::Bank::find(bankName);
}

