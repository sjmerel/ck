#pragma once

#include <math.h>

struct Vector3
{
    float x;
    float y;
    float z;

    Vector3() {}
    Vector3(float xx, float yy, float zz) : x(xx), y(yy), z(zz) {}

    ////////////////////////////////////////

    static void add(const Vector3& v1, const Vector3& v2, Vector3& result)
    {
        result.x = v1.x + v2.x;
        result.y = v1.y + v2.y;
        result.z = v1.z + v2.z;
    }

    static void subtract(const Vector3& v1, const Vector3& v2, Vector3& result)
    {
        result.x = v1.x - v2.x;
        result.y = v1.y - v2.y;
        result.z = v1.z - v2.z;
    }

    static void multiply(const Vector3& v, float s, Vector3& result)
    {
        result.x = v.x * s;
        result.y = v.y * s;
        result.z = v.z * s;
    }

    static void cross(const Vector3& v1, const Vector3& v2, Vector3& result)
    {
        result.x = v1.y * v2.z - v1.z * v2.y;
        result.y = v1.z * v2.x - v1.x * v2.z;
        result.z = v1.x * v2.y - v1.y * v2.x;
    }

    static float dot(const Vector3& v1, const Vector3& v2)
    {
        return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
    }

    static float mag2(const Vector3& v)
    {
        return dot(v, v);
    }

    static float mag(const Vector3& v)
    {
        return sqrtf(mag2(v));
    }

    static void normalize(const Vector3& v, Vector3& result)
    {
        float invMag = 1.0f / mag(v);
        result.x = v.x*invMag;
        result.y = v.y*invMag;
        result.z = v.z*invMag;
    }
};
