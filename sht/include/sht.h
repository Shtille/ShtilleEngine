#pragma once
#ifndef __SHT_INCLUDE_SHT_H__
#define __SHT_INCLUDE_SHT_H__

#include "../common/platform.h"

// Choose renderer
#if defined(RENDERER_OPENGL)

#include "../application/opengl/opengl_application.h"

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
app->Run();												\
delete app;												\
return 0;												\
}

#elif defined(TARGET_LINUX) || defined(TARGET_MAC)

#define DECLARE_MAIN(UserApplicationClass)				\
int main(int argc, const char ** argv)					\
{														\
UserApplicationClass *app = new UserApplicationClass();	\
app->Run();												\
delete app;												\
return 0;												\
}

#endif

#endif