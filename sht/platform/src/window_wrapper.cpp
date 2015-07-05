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