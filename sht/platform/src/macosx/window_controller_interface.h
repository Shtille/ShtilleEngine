#pragma once
#ifndef __SHT_PLATFORM_WINDOW_CONTROLLER_INTERFACE_H__
#define __SHT_PLATFORM_WINDOW_CONTROLLER_INTERFACE_H__

void PlatformWindowMakeWindowedImpl(void *instance);
void PlatformWindowMakeFullscreenImpl(void *instance);
void PlatformWindowResizeImpl(void *instance, int width, int height);
void PlatformWindowSetTitleImpl(void *instance, const char *title);

#endif