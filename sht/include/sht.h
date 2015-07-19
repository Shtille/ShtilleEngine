#pragma once
#ifndef __SHT_INCLUDE_SHT_H__
#define __SHT_INCLUDE_SHT_H__

#include "../common/platform.h"

#if defined(TARGET_WINDOWS) || defined(TARGET_MAC) || defined(TARGET_LINUX)
#define RENDERER_OPENGL // other isn't emplemented
#elif defined(TARGET_IOS) || defined(TARGET_ANDROID)
#define RENDERER_OPENGLES
#endif

// Choose renderer
#if defined(RENDERER_OPENGL)

#include "../application/opengl/opengl_application.h"

#elif defined(RENDERER_OPENGLES)

#include "../application/opengl/opengles_application.h"

#elif defined(RENDERER_DIRECTX)

#include "../application/directx_application.h"

#else

#pragma message("There is no renderer had been specified. Using OpenGL renderer as default.") 
#include "../application/opengl/opengl_application.h"

#endif

// Main function declaration macros
#if defined(TARGET_WINDOWS)

#define DECLARE_MAIN(UserApplicationClass)				\
int WINAPI WinMain(HINSTANCE hInstance,					\
				   HINSTANCE hPrevInstance,				\
				   LPSTR lpCmdLine,						\
				   int nCmdShow)						\
{														\
UserApplicationClass *app = new UserApplicationClass();	\
int r = app->Run(0,0);									\
delete app;												\
return r;												\
}

#elif defined(TARGET_LINUX) || defined(TARGET_MAC)

#define DECLARE_MAIN(UserApplicationClass)				\
int main(int argc, const char ** argv)					\
{														\
UserApplicationClass *app = new UserApplicationClass();	\
int r = app->Run(argc, argv);							\
delete app;												\
return r;												\
}

#endif

#endif