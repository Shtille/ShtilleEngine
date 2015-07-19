#pragma once
#ifndef __SHT_PLATFORM_WINDOW_STRUCT_H__
#define __SHT_PLATFORM_WINDOW_STRUCT_H__

#include "../../common/platform.h"

struct PlatformWindow {
#ifdef TARGET_WINDOWS
	HWND hwnd;
	HDC dc;
	HGLRC rc;
#endif
};

#endif