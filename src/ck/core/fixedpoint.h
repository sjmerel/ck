#pragma once

#include "ck/core/platform.h"
#include "ck/core/types.h"

namespace Cki
{


template <int BITS>
struct IntType {};

template <> struct IntType<8>  { typedef int8 Type; };
template <> struct IntType<16> { typedef int16 Type; };
template <> struct IntType<32> { typedef int32 Type; };
template <> struct IntType<64> { typedef int64 Type; };

template <int INTBITS, int FRACBITS>
struct FixedPoint
{
    enum { SizeBits = INTBITS + FRACBITS };
    enum { IntegerBits = INTBITS };
    enum { FractionalBits = FRACBITS };

    static float getResolution() { return 1.0f / (1 << FractionalBits); }

    typedef FixedPoint<INTBITS,FRACBITS> FixedType;
    typedef typename IntType<SizeBits>::Type RepType;
    typedef typename IntType<SizeBits*2>::Type ProductType;

    static const RepType k_oneRep = 1 << FractionalBits;

    ////////////////////////////////////////

    static RepType fromFloat(float value)
    {
        return (RepType) (value * (1 << FractionalBits) + (value >= 0.0f ? 0.5f : -0.5f));
    }

    static float toFloat(RepType value)
    {
        return (float) value / (1 << FractionalBits);
    }

    static RepType intPart(RepType value)
    {
        return value >> FractionalBits;
    }


    static RepType add(RepType a, RepType b)
    {
        return a + b;
    }

    static RepType subtract(RepType a, RepType b)
    {
        return a - b;
    }

    static RepType multiply(RepType a, RepType b)
    {
        return (RepType) (((ProductType) a * (ProductType) b ) >> FractionalBits);
    }

    static RepType divide(RepType a, RepType b)
    {
        return (RepType) (((ProductType) a << FractionalBits) / b);
    }


    ////////////////////////////////////////

    RepType m_rep;

    FixedPoint(RepType rep) : m_rep(rep) {}

    FixedPoint(float f) : m_rep(fromFloat(f)) {}

    float toFloat() const { return toFloat(m_rep); } 

    RepType intPart() const { return intPart(m_rep); }

    FixedType operator+(FixedType a) const
    {
        return FixedType(add(m_rep, a));
    }

    FixedType operator+=(FixedType a)
    {
        m_rep = add(m_rep, a.m_rep);
        return FixedType(m_rep);
    }

    FixedType operator-(FixedType a) const
    {
        return FixedType(subtract(m_rep, a));
    }

    FixedType operator-=(FixedType a)
    {
        m_rep = subtract(m_rep, a.m_rep);
        return FixedType(m_rep);
    }

    FixedType operator*(FixedType a) const
    {
        return FixedType(multiply(m_rep, a.m_rep));
    }

    FixedType operator*=(FixedType a)
    {
        m_rep = multiply(m_rep, a.m_rep);
        return FixedType(m_rep);
    }
};

typedef FixedPoint<8,24> Fixed8_24;
typedef FixedPoint<8,24>::RepType Fixed8_24_t;

typedef FixedPoint<16,16> Fixed16_16;
typedef FixedPoint<16,16>::RepType Fixed16_16_t;



}
