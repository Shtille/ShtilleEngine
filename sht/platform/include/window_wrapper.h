#pragma once
#ifndef __SHT_PLATFORM_WINDOW_WRAPPER_H__
#define __SHT_PLATFORM_WINDOW_WRAPPER_H__

#include <string>

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

// OpenGL context functions
bool PlatformInitOpenGLContext(int color_bits, int depth_bits);
void PlatformDeinitOpenGLContext();
void PlatformSwapBuffers();

// Cursor functions
void PlatformSetCursorPos(int x, int y);
void PlatformGetCursorPos(int& x, int& y);
void PlatformMouseToCenter();

// Clipboard functions
void PlatformSetClipboardText(const char *text);
std::string PlatformGetClipboardText(); //!< string should be freed via free() function

#endif