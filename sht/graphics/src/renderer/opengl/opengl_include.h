#pragma once
#ifndef __SHT_GRAPHICS_OPENGL_INCLUDE_H__
#define __SHT_GRAPHICS_OPENGL_INCLUDE_H__

#include "../../../../common/platform.h"

#ifdef TARGET_WINDOWS
#include "../../../../thirdparty/glew/include/GL/glew.h"
#elif defined(TARGET_MAC)
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#else
#error OpenGL includes haven't been defined
#endif

#endif