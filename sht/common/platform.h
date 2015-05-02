#pragma once
#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#if defined(_WIN32) // Windows

#define TARGET_WINDOWS

#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <Windows.h>
#include "../system/include/memory_leaks.h"

#elif defined(__unix__) || defined(__unix) // Linux

#define TARGET_UNIX

#elif defined(__APPLE__) && defined(__MACH__) // Mac OS X

#define TARGET_MAC

#else

#error Unknown target platform

#endif

#include <assert.h>

#endif