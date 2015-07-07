#include "../include/window_wrapper.h"
#include "../../common/platform.h"

#ifdef TARGET_MAC
#include "./macosx/window_controller_interface.h"
#endif

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
const char* PlatformGetClipboardText()
{
    return PlatformGetClipboardTextImpl(g_window_controller);
}