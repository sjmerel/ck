#pragma once

#include "ck/core/platform.h"
#include "ck/core/math.h"

namespace Cki
{


struct VolumeMatrix
{
public:
    float ll;
    float lr;
    float rl;
    float rr;

    VolumeMatrix() {}
    VolumeMatrix(float ll, float rr);
    VolumeMatrix(float ll, float lr, float rl, float rr);

    void set(float ll, float lr, float rl, float rr);
    void setIdentity();
    bool isIdentity(float tol = 0.001f) const;
    void setScalar(float s);
    bool isScalar(float tol = 0.001f) const;
    void setDiagonal(float ll, float rr);
    bool isDiagonal(float tol = 0.001f) const;
    void setZero();
    bool isZero(float tol = 0.001f) const;

    VolumeMatrix operator+(const VolumeMatrix&) const;
    VolumeMatrix& operator+=(const VolumeMatrix&);
    VolumeMatrix operator-(const VolumeMatrix&) const;
    VolumeMatrix& operator-=(const VolumeMatrix&);
    VolumeMatrix operator*(float) const;
    VolumeMatrix& operator*=(float);
    VolumeMatrix operator/(float) const;
    VolumeMatrix& operator/=(float);

    bool equals(const VolumeMatrix&, float tol) const;

//    void setVolumePan(float volume, float pan); // equal-power
//    void getVolumePan(float& volume, float& pan) const;

    void setPan(float); // equal-power
    float getPan() const; // undefined if set other than with setPan()!

//    void setStereoVolumePan(float volume, float pan); // balance
//    void getStereoVolumePan(float& volume, float& pan) const;

    void setStereoPan(float); // balance
    float getStereoPan() const; // undefined if set other than with setStereoPan()!

    float getPower() const;

    // static versions of all above that return a temporary matrix or are non-const
    static void add(VolumeMatrix& result, const VolumeMatrix& a, const VolumeMatrix& b);
    static void subtract(VolumeMatrix& result, const VolumeMatrix& a, const VolumeMatrix& b);
    static void scale(VolumeMatrix& result, const VolumeMatrix& a, float s);

};

////////////////////////////////////////

inline
VolumeMatrix::VolumeMatrix(float ll_, float rr_) :
    ll(ll_),
    lr(0.0f),
    rl(0.0f),
    rr(rr_)
{}

inline
VolumeMatrix::VolumeMatrix(float ll_, float lr_, float rl_, float rr_) :
    ll(ll_),
    lr(lr_),
    rl(rl_),
    rr(rr_)
{}

inline
void VolumeMatrix::set(float ll_, float lr_, float rl_, float rr_)
{
    ll = ll_;
    lr = lr_;
    rl = rl_;
    rr = rr_;
}

inline
void VolumeMatrix::setIdentity()
{
    ll = 1.0f;
    lr = 0.0f;
    rl = 0.0f;
    rr = 1.0f;
}

inline
bool VolumeMatrix::isIdentity(float tol) const
{
    return Math::floatEqual(ll, 1.0f, tol) &&
           Math::floatEqual(lr, 0.0f, tol) &&
           Math::floatEqual(rl, 0.0f, tol) &&
           Math::floatEqual(rr, 1.0f, tol);
}

inline
void VolumeMatrix::setScalar(float s)
{
    ll = s;
    lr = 0.0f;
    rl = 0.0f;
    rr = s;
}

inline
bool VolumeMatrix::isScalar(float tol) const
{
    return Math::floatEqual(ll, rr, tol) &&
           Math::floatEqual(lr, 0.0f, tol) &&
           Math::floatEqual(rl, 0.0f, tol);
}

inline
void VolumeMatrix::setDiagonal(float l, float r)
{
    ll = l;
    lr = 0.0f;
    rl = 0.0f;
    rr = r;
}

inline
bool VolumeMatrix::isDiagonal(float tol) const
{
    return Math::floatEqual(lr, 0.0f, tol) &&
           Math::floatEqual(rl, 0.0f, tol);
}

inline
void VolumeMatrix::setZero()
{
    ll = 0.0f;
    lr = 0.0f;
    rl = 0.0f;
    rr = 0.0f;
}

inline
bool VolumeMatrix::isZero(float tol) const
{
    return Math::floatEqual(ll, 0.0f, tol) &&
           Math::floatEqual(lr, 0.0f, tol) &&
           Math::floatEqual(rl, 0.0f, tol) &&
           Math::floatEqual(rr, 0.0f, tol);
}

inline 
VolumeMatrix VolumeMatrix::operator+(const VolumeMatrix& a) const
{
    VolumeMatrix result;
    add(result, *this, a);
    return result;
}

inline 
VolumeMatrix& VolumeMatrix::operator+=(const VolumeMatrix& a)
{
    add(*this, *this, a);
    return *this;
}

inline 
VolumeMatrix VolumeMatrix::operator-(const VolumeMatrix& a) const
{
    VolumeMatrix result;
    subtract(result, *this, a);
    return result;
}

inline 
VolumeMatrix& VolumeMatrix::operator-=(const VolumeMatrix& a)
{
    subtract(*this, *this, a);
    return *this;
}

inline 
VolumeMatrix VolumeMatrix::operator*(float s) const
{
    VolumeMatrix result;
    scale(result, *this, s);
    return result;
}

inline 
VolumeMatrix& VolumeMatrix::operator*=(float s)
{
    scale(*this, *this, s);
    return *this;
}

inline 
VolumeMatrix VolumeMatrix::operator/(float s) const
{
    VolumeMatrix result;
    scale(result, *this, 1.0f/s);
    return result;
}

inline 
VolumeMatrix& VolumeMatrix::operator/=(float s)
{
    scale(*this, *this, 1.0f/s);
    return *this;
}

inline
bool VolumeMatrix::equals(const VolumeMatrix& other, float tol) const
{
    return Math::floatEqual(other.ll, ll, tol) &&
           Math::floatEqual(other.rr, rr, tol) &&
           Math::floatEqual(other.lr, lr, tol) &&
           Math::floatEqual(other.rl, rl, tol);
}

inline
void VolumeMatrix::add(VolumeMatrix& result, const VolumeMatrix& a, const VolumeMatrix& b)
{
    result.ll = a.ll + b.ll;
    result.lr = a.lr + b.lr;
    result.rl = a.rl + b.rl;
    result.rr = a.rr + b.rr;
}

inline
void VolumeMatrix::subtract(VolumeMatrix& result, const VolumeMatrix& a, const VolumeMatrix& b)
{
    result.ll = a.ll - b.ll;
    result.lr = a.lr - b.lr;
    result.rl = a.rl - b.rl;
    result.rr = a.rr - b.rr;
}

inline
void VolumeMatrix::scale(VolumeMatrix& result, const VolumeMatrix& a, float s)
{
    result.ll = a.ll * s;
    result.rl = a.rl * s;
    result.lr = a.lr * s;
    result.rr = a.rr * s;
}


}
