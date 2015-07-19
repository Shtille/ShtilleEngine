#include "../include/window_wrapper.h"
#include "../include/window_controller_interface.h"
#include "../../common/platform.h"

extern void * g_window_controller;

void PlatformWindowMakeWindowed()
{
    PlatformWindowMakeWindowedImpl(g_window_controller);
}
void PlatformWindowMakeFullscreen()
{
    PlatformWindowMakeFullscreenImpl(g_window_controller);
}
void PlatformWindowResize(int width, int height)
{
    PlatformWindowResizeImpl(g_window_controller, width, height);
}
void PlatformWindowSetTitle(const char *title)
{
    PlatformWindowSetTitleImpl(g_window_controller, title);
}
void PlatformWindowIconify()
{
    PlatformWindowIconifyImpl(g_window_controller);
}
void PlatformWindowRestore()
{
    PlatformWindowRestoreImpl(g_window_controller);
}
void PlatformWindowShow()
{
    PlatformWindowShowImpl(g_window_controller);
}
void PlatformWindowHide()
{
    PlatformWindowHideImpl(g_window_controller);
}
void PlatformWindowTerminate()
{
    PlatformWindowTerminateImpl(g_window_controller);
}
bool PlatformInitOpenGLContext(int color_bits, int depth_bits)
{
	return PlatformInitOpenGLContextImpl(g_window_controller, color_bits, depth_bits);
}
void PlatformDeinitOpenGLContext()
{
	PlatformDeinitOpenGLContextImpl(g_window_controller);
}
void PlatformSwapBuffers()
{
	PlatformSwapBuffersImpl(g_window_controller);
}
void PlatformSetCursorPos(int x, int y)
{
    PlatformSetCursorPosImpl(g_window_controller, x, y);
}
void PlatformGetCursorPos(int& x, int& y)
{
    PlatformGetCursorPosImpl(g_window_controller, x, y);
}
void PlatformMouseToCenter()
{
    PlatformMouseToCenterImpl(g_window_controller);
}
void PlatformSetClipboardText(const char *text)
{
    PlatformSetClipboardTextImpl(g_window_controller, text);
}
std::string PlatformGetClipboardText()
{
    return PlatformGetClipboardTextImpl(g_window_controller);
}