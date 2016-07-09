#pragma once
#ifndef __SHT_PLATFORM_WINDOW_WRAPPER_H__
#define __SHT_PLATFORM_WINDOW_WRAPPER_H__

#include <string>

// Window functions
void PlatformWindowMakeWindowed();
bool PlatformWindowMakeFullscreen();
void PlatformWindowCenter();
void PlatformWindowResize(int width, int height);
void PlatformWindowSetTitle(const char *title);
void PlatformWindowIconify();
void PlatformWindowRestore();
void PlatformWindowShow();
void PlatformWindowHide();
void PlatformWindowTerminate();

// OpenGL context functions
bool PlatformInitOpenGLContext(int color_bits, int depth_bits, int stencil_bits);
void PlatformDeinitOpenGLContext();
void PlatformSwapBuffers();

// Cursor functions
void PlatformSetCursorPos(float x, float y);
void PlatformGetCursorPos(float& x, float& y);
void PlatformMouseToCenter();
void PlatformShowCursor();
void PlatformHideCursor();

// Clipboard functions
void PlatformSetClipboardText(const char *text);
std::string PlatformGetClipboardText(); //!< string should be freed via free() function

// Other
void PlatformChangeDirectoryToResources();

#endif