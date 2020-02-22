#pragma once

#include "ck/core/platform.h"
#include "ck/core/math.h"
#include "ck/core/debug.h"

namespace Cki
{


class Vector3
{
public:
    float x;
    float y;
    float z;

    Vector3() {}
    Vector3(float x, float y, float z);

    void set(float x, float y, float z);

    Vector3 operator+(const Vector3&) const;
    Vector3& operator+=(const Vector3&);
    Vector3 operator-(const Vector3&) const;
    Vector3& operator-=(const Vector3&);
    Vector3 operator*(float) const;
    Vector3& operator*=(float);
    Vector3 operator/(float) const;
    Vector3& operator/=(float);
    Vector3 operator-() const;

    float getMagnitude() const;
    float getMagnitudeSquared() const;
    void normalize();
    float dot(const Vector3&) const;
    Vector3 cross(const Vector3&) const;

    // static versions of all above that return a temporary Vector3 or are non-const
    static void add(Vector3& result, const Vector3& a, const Vector3& b);
    static void subtract(Vector3& result, const Vector3& a, const Vector3& b);
    static void scale(Vector3& result, const Vector3& a, float s);
    static void normalize(Vector3& result, const Vector3& a);
    static void cross(Vector3& result, const Vector3& a, const Vector3& b);
    static void negate(Vector3& result, const Vector3& a);
};

////////////////////////////////////////

inline
Vector3::Vector3(float x_, float y_, float z_) : 
    x(x_), 
    y(y_), 
    z(z_) 
{}

inline
void Vector3::set(float x_, float y_, float z_)
{
    x = x_;
    y = y_;
    z = z_;
}

inline 
Vector3 Vector3::operator+(const Vector3& a) const
{
    Vector3 result;
    add(result, *this, a);
    return result;
}

inline 
Vector3& Vector3::operator+=(const Vector3& a)
{
    add(*this, *this, a);
    return *this;
}

inline 
Vector3 Vector3::operator-(const Vector3& a) const
{
    Vector3 result;
    subtract(result, *this, a);
    return result;
}

inline 
Vector3& Vector3::operator-=(const Vector3& a)
{
    subtract(*this, *this, a);
    return *this;
}

inline 
Vector3 Vector3::operator*(float s) const
{
    Vector3 result;
    scale(result, *this, s);
    return result;
}

inline 
Vector3& Vector3::operator*=(float s)
{
    scale(*this, *this, s);
    return *this;
}

inline 
Vector3 Vector3::operator/(float s) const
{
    Vector3 result;
    scale(result, *this, 1.0f/s);
    return result;
}

inline 
Vector3& Vector3::operator/=(float s)
{
    scale(*this, *this, 1.0f/s);
    return *this;
}

inline
Vector3 Vector3::operator-() const
{
    Vector3 result;
    negate(result, *this);
    return result;
}

inline 
float Vector3::getMagnitude() const
{
    return Math::sqrt(getMagnitudeSquared());
}

inline 
float Vector3::getMagnitudeSquared() const
{
    return dot(*this);
}

inline 
float Vector3::dot(const Vector3& a) const
{
    return x*a.x + y*a.y + z*a.z;
}

inline 
Vector3 Vector3::cross(const Vector3& a) const
{
    Vector3 result;
    cross(result, *this, a);
    return result;
}

inline 
void Vector3::normalize()
{
    normalize(*this, *this);
}

inline
void Vector3::add(Vector3& result, const Vector3& a, const Vector3& b)
{
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
}

inline
void Vector3::subtract(Vector3& result, const Vector3& a, const Vector3& b)
{
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
}

inline 
void Vector3::scale(Vector3& result, const Vector3& a, float s)
{
    result.x = a.x * s;
    result.y = a.y * s;
    result.z = a.z * s;
}

inline 
void Vector3::normalize(Vector3& result, const Vector3& a)
{
    float len = a.getMagnitude();
    CK_ASSERT(len > 1.0e-15f);

    // add small value so very small vectors don't cause NaN 
    // (though they will not be properly normalized)
    // see http://www.gamasutra.com/view/news/36746/Opinion_Practical_Floating_Point.php
    len += 1.0e-37f;

    float s = 1.0f / len;
    result.x = a.x * s;
    result.y = a.y * s;
    result.z = a.z * s;
}

inline
void Vector3::cross(Vector3& result, const Vector3& a, const Vector3& b)
{
    result.x = a.y*b.z - a.z*b.y;
    result.y = a.z*b.x - a.x*b.z;
    result.z = a.x*b.y - a.y*b.x;
}

inline
void Vector3::negate(Vector3& result, const Vector3& a)
{
    result.set(-a.x, -a.y, -a.z);
}


}
