#pragma once
#ifndef __SHT_PLATFORM_WINDOW_CONTROLLER_INTERFACE_H__
#define __SHT_PLATFORM_WINDOW_CONTROLLER_INTERFACE_H__

#include <string>

// Inner functions
bool PlatformInitImpl(void *instance);
void PlatformTerminateImpl(void *instance);
void PlatformAdjustVideoSettingsImpl();
void PlatformRestoreVideoSettingsImpl();
bool PlatformWindowCreateImpl(void *instance);
void PlatformWindowDestroyImpl(void *instance);
bool PlatformNeedQuitImpl(void *instance);
void PlatformPollEventsImpl(void *instance);

// Window functions
void PlatformWindowMakeWindowedImpl(void *instance);
void PlatformWindowMakeFullscreenImpl(void *instance);
void PlatformWindowCenterImpl(void *instance);
void PlatformWindowResizeImpl(void *instance, int width, int height);
void PlatformWindowSetTitleImpl(void *instance, const char *title);
void PlatformWindowIconifyImpl(void *instance);
void PlatformWindowRestoreImpl(void *instance);
void PlatformWindowShowImpl(void *instance);
void PlatformWindowHideImpl(void *instance);
void PlatformWindowTerminateImpl(void *instance);

// OpenGL context functions
bool PlatformInitOpenGLContextImpl(void *instance, int color_bits, int depth_bits, int stencil_bits);
void PlatformDeinitOpenGLContextImpl(void *instance);
void PlatformSwapBuffersImpl(void *instance);

// Cursor functions
void PlatformSetCursorPosImpl(void *instance, float x, float y);
void PlatformGetCursorPosImpl(void *instance, float& x, float& y);
void PlatformMouseToCenterImpl(void *instance);
void PlatformShowCursorImpl(void *instance);
void PlatformHideCursorImpl(void *instance);

// Clipboard functions
void PlatformSetClipboardTextImpl(void *instance, const char *text);
std::string PlatformGetClipboardTextImpl(void *instance);

// Other
void PlatformChangeDirectoryToResourcesImpl();

#endif