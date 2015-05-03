#pragma once
#ifndef __SHT_STRING_H__
#define __SHT_STRING_H__

#include "platform.h"
#include <string.h>

#ifndef TARGET_WINDOWS

#define strcpy_s strcpy
#define strcat_s strcat
#define sprintf_s sprintf

#endif

#endif