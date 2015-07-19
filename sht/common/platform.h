#pragma once
#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#if defined(_WIN32) // Windows

#define TARGET_WINDOWS

#ifdef _MSC_VER
#include <SDKDDKVer.h>
#endif

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <Windows.h>
#include "../system/include/memory_leaks.h"
#include <stddef.h>

#elif defined(__unix__) || defined(__unix) // Linux

#define TARGET_UNIX

#elif defined(__APPLE__) && defined(__MACH__) // Mac OS X

	#include <TargetConditionals.h>
	#if TARGET_IPHONE_SIMULATOR == 1
	/* iOS in Xcode simulator */
	#elif TARGET_OS_IPHONE == 1
	/* iOS on iPhone, iPad, etc. */
	#define TARGET_IOS
	#elif TARGET_OS_MAC == 1
	/* OSX */
	#define TARGET_MAC
	#endif
#else

#error Unknown target platform

#endif

#if defined(TARGET_WINDOWS) || defined(TARGET_LINUX) || defined(TARGET_MAC)
#define TARGET_DESKTOP
#else
#define TARGET_MOBILE
#endif

#include <assert.h>

#endif