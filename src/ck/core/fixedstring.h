#pragma once

#include "ck/core/platform.h"
#include "ck/core/string.h"

namespace Cki
{


template <int N>
class FixedString : public String
{
public:
    FixedString();
    FixedString(const char*);
    FixedString(const String&);
//    FixedString(const char*, int n);
//    FixedString(const String&, int n);

    FixedString(const FixedString<N>&);
    FixedString& operator=(const FixedString<N>&);

    ~FixedString();

    enum { k_bufSize = N }; 
    enum { k_capacity = N-1 }; // not including null

private:
    char m_array[N]; // storage for N chars (string length N-1)
};

typedef FixedString<32> FixedString32;
typedef FixedString<64> FixedString64; 
typedef FixedString<128> FixedString128; 
typedef FixedString<256> FixedString256; 



}
