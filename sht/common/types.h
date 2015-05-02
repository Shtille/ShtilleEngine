#pragma once
#ifndef __TYPES_H__
#define __TYPES_H__

#ifndef _countof
#define _countof(x) (sizeof(x)/sizeof(x[0]))
#endif

// signed types
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;
// unsigned types
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
// float types
typedef float f32;
typedef double f64;

#endif