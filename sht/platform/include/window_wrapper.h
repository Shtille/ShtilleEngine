#pragma once
#ifndef __SHT_PLATFORM_WINDOW_WRAPPER_H__
#define __SHT_PLATFORM_WINDOW_WRAPPER_H__

// Window functions
void PlatformWindowMakeWindowed();
void PlatformWindowMakeFullscreen();
void PlatformWindowResize(int width, int height);
void PlatformWindowSetTitle(const char *title);
void PlatformWindowIconify();
void PlatformWindowRestore();
void PlatformWindowShow();
void PlatformWindowHide();
void PlatformWindowTerminate();

// Cursor functions
void PlatformSetCursorPos(int x, int y);
void PlatformGetCursorPos(int& x, int& y);
void PlatformMouseToCenter();

// Clipboard functions
void PlatformSetClipboardText(const char *text);
const char* PlatformGetClipboardText(); //!< string should be freed via free() function

#endif