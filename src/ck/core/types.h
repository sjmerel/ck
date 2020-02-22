#pragma once

#include "ck/core/platform.h"

namespace Cki
{



// shorthand for unsigned types

typedef unsigned char       uchar;
typedef unsigned short      ushort;
typedef unsigned int        uint;
typedef unsigned long       ulong;



// explicitly sized integer types

typedef signed char         int8;
typedef unsigned char       uint8;

typedef signed short        int16;
typedef unsigned short      uint16;

typedef signed int          int32;
typedef unsigned int        uint32;

typedef signed long long    int64;
typedef unsigned long long  uint64;



// explicitly sized bools

typedef uint8               bool8;
typedef uint16              bool16;
typedef uint32              bool32;


// explicitly sized floats

typedef float               float32;
typedef double              float64;


// use this when twiddling bytes in memory

typedef uint8               byte;



// integer type min/max values

#define CK_INT8_MAX     0x7f
#define CK_INT8_MIN     (-0x7f - 1)
#define CK_UINT8_MAX    0xff

#define CK_INT16_MAX    0x7fff
#define CK_INT16_MIN    (-0x7fff - 1)
#define CK_UINT16_MAX   0xffff

#define CK_INT32_MAX    0x7fffffff
#define CK_INT32_MIN    (-0x7fffffff - 1)
#define CK_UINT32_MAX   0xffffffff

#define CK_INT64_MAX    0x7fffffffffffffffLL
#define CK_INT64_MIN    (-0x7fffffffffffffffLL - 1)
#define CK_UINT64_MAX   0xffffffffffffffffULL

#ifndef NULL
#  define NULL 0
#endif

#define CK_UNUSED(x) (void) x

#define CK_ARRAY_COUNT(x) sizeof(x)/sizeof(x[0])

#define CK_STRINGIZE(x) #x
#define CK_STRINGIZE_VALUE(x) CK_STRINGIZE(x)
#define CK_CONCAT(x, y) x##y
#define CK_CONCAT_VALUE(x, y) CK_CONCAT(x, y)


}
