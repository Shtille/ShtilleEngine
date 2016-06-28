#pragma once
#ifndef __SHT_PLATFORM_WINDOW_STRUCT_H__
#define __SHT_PLATFORM_WINDOW_STRUCT_H__

#include "../../common/platform.h"

struct PlatformWindow {
#ifdef TARGET_WINDOWS
	HWND hwnd;
	HDC dc;
	HGLRC rc;

	HINSTANCE instance;

	struct WindowState {
		RECT rect;
		LONG_PTR style;
		LONG_PTR ex_style;
	};
	WindowState old_state;
	WindowState current_state;

	HICON icon;
#endif
	bool need_quit;
};

#endif