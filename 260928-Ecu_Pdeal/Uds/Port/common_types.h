#ifndef COMMON_TYPES_H_
#define COMMON_TYPES_H_

#include <stdint.h>

typedef uint8_t boolean;
//#ifndef boolean
//typedef uint8_t boolean;
//#endif

typedef unsigned int AddrType;

typedef signed char sint8_t;
typedef signed short sint16_t;
typedef signed int sint32_t;
typedef signed long long sint64_t;

typedef sint8_t sint8;
typedef sint32_t sint32;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef NULL_PTR
#define NULL_PTR ((void *)0)
#endif

#endif
