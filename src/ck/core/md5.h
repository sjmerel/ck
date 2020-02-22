#pragma once

#include "ck/core/platform.h"
#include "ck/core/types.h"

namespace Cki
{


class MD5
{
public:
    MD5();

    void update(const byte* data, int length);
    void finish();

    const byte* getHash() const { return m_hash; }
    enum { k_hashLen = 16 };

private:

    struct MD5_CTX
    {
        uint32 i[2];                   /* number of _bits_ handled mod 2^64 */
        uint32 buf[4];                                    /* scratch buffer */
        unsigned char in[64];                              /* input buffer */
        unsigned char digest[16];     /* actual digest after MD5Final call */
    };

    static void MD5Init(MD5_CTX* mdContext);
    static void MD5Update(MD5_CTX* mdContext, unsigned char* inBuf, unsigned int inLen);
    static void MD5Final(MD5_CTX* mdContext);

    MD5_CTX m_context;
    byte m_hash[16];
};


}
