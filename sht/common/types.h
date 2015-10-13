#pragma once
#ifndef __TYPES_H__
#define __TYPES_H__

#ifndef _countof
#define _countof(x) (sizeof(x)/sizeof(x[0]))
#endif

#include <stddef.h>
#include <stdint.h>

// signed types
typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
// unsigned types
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
// float types
typedef float  f32;
typedef double f64;

#endif