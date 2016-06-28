#pragma once
#ifndef __SHT_PLATFORM_INNER_H__
#define __SHT_PLATFORM_INNER_H__

// Basic platform functions that shouldn't be visible from includes

bool PlatformInit();
void PlatformTerminate();
void PlatformAdjustVideoSettings();
void PlatformRestoreVideoSettings();
bool PlatformWindowCreate();
void PlatformWindowDestroy();
bool PlatformNeedQuit();
void PlatformPollEvents();

#endif