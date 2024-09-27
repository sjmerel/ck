#pragma once

#include "ck/core/platform.h"

#include <math.h>
#include <stdlib.h>

#if CK_PLATFORM_WIN || CK_PLATFORM_WP8
#  undef min
#  undef max
#endif

namespace Cki
{


namespace Math
{

    // calculated with https://www.cs.indiana.edu/~flaradam/applications/Calculator/CalBig/ts/SC.html
    static const float k_pi =          3.14159265358979323846f; // pi
    static const float k_2pi =         6.28318530717958647693f; // 2*pi
    static const float k_pi_2 =        1.57079632679489661923f; // pi/2
    static const float k_pi_4 =        0.78539816339744830962f; // pi/4
    static const float k_sqrt2 =       1.41421356237309504880f; // sqrt(2)
    static const float k_1_sqrt2 =     0.70710678118654752440f; // 1/sqrt(2)


    template <typename T> 
    inline
    T abs(T a) 
    { 
        return (a < 0 ? -a : a); 
    }

    inline 
    bool floatEqual(float f1, float f2, float tol)
    {
        return abs(f1 - f2) <= tol;
    }

    template <typename T>
    inline 
    T clamp(T value, T min, T max)
    {
        if (value < min)
        {
            return min;
        }
        else if (value > max)
        {
            return max;
        }
        else
        {
            return value;
        }
    }

    inline
    int wrap(int value, int min, int max)
    {
        value = (value - min) % (max - min);
        return (value < 0 ? max + value : min + value);
    }

    template <typename T> 
    inline
    T min(T a, T b) 
    { 
        return (b < a ? b : a); 
    }

    template <typename T> 
    inline
    T min(T a, T b, T c) 
    { 
        return min(min(a, b), c);
    }

    template <typename T> 
    inline
    T min(T a, T b, T c, T d) 
    { 
        return min(min(a, b), min(c, d));
    }

    template <typename T> 
    inline
    T max(T a, T b) 
    { 
        return (b < a ? a : b); 
    }

    template <typename T> 
    inline
    T max(T a, T b, T c) 
    { 
        return max(max(a, b), c);
    }

    template <typename T> 
    inline
    T max(T a, T b, T c, T d) 
    { 
        return max(max(a, b), max(c, d));
    }

    template <typename T>
    inline
    int sign(T x)
    {
        return (x > 0) - (x < 0);
    }

    inline int round(float value)
    {
        return (value > 0.0f) ? (int) (value + 0.5f) : (int) (value - 0.5f);
    }

    inline int round(double value)
    {
        return (value > 0.0) ? (int) (value + 0.5) : (int) (value - 0.5);
    }

    inline float floor(float value)
    {
        return (float) ::floor(value);
    }

    inline float sin(float value)
    {
        return sinf(value);
    }

    inline float asin(float value)
    {
        return asinf(value);
    }

    inline float cos(float value)
    {
        return cosf(value);
    }

    inline float acos(float value)
    {
        return acosf(value);
    }

    inline float tan(float value)
    {
        return tanf(value);
    }

    inline float sqrt(float value)
    {
        return sqrtf(value);
    }

    inline float pow(float base, float exponent)
    {
        return powf(base, exponent);
    }

    inline float log10(float value)
    {
        return log10f(value);
    }

    inline float exp(float value)
    {
        return expf(value);
    }

    inline float ln(float value)
    {
        return logf(value);
    }

    inline float log(float base, float value)
    {
        return ln(value) / ln(base);
    }

    inline float interp(float f, float min, float max)
    {
        return min + f * (max-min);
    }

    inline float interpInv(float f, float min, float max)
    {
        return (f - min)/(max-min);
    }

    inline float remap(float f, float min, float max, float newMin, float newMax)
    {
        return interp(interpInv(f, min, max), newMin, newMax);
    }

    // returns number in range [min, max)
    inline int randomInt(int min, int max)
    {
        return (rand() % (max - min)) + min;
    }

    inline float randomFloat(float min, float max)
    {
        return rand() * (max-min) / float(RAND_MAX);
    }

    ////////////////////////////////////////

    template <int a, int b>
    struct Max
    {
        static const int value = (a > b ? a : b);
    };

    template <int a, int b, int c>
    struct Max3
    {
        static const int value = Max<Max<a, b>::value, c>::value;
    };

    template <int a, int b, int c, int d>
    struct Max4
    {
        static const int value = Max<Max<a, b>::value, Max<c, d>::value>::value;
    };

    template <int a, int b>
    struct Min
    {
        static const int value = (a < b ? a : b);
    };

    template <int a, int b, int c>
    struct Min3
    {
        static const int value = Min<Min<a, b>::value, c>::value;
    };

    template <int a, int b, int c, int d>
    struct Min4
    {
        static const int value = Min<Min<a, b>::value, Min<c, d>::value>::value;
    };

} 


}
