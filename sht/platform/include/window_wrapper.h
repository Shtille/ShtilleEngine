#pragma once
#ifndef __SHT_PLATFORM_WINDOW_WRAPPER_H__
#define __SHT_PLATFORM_WINDOW_WRAPPER_H__

void PlatformWindowMakeWindowed();
void PlatformWindowMakeFullscreen();
void PlatformWindowResize(int width, int height);
void PlatformWindowSetTitle(const char *title);

#endif