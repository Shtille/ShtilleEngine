#include "opengl_application.h"
#include "../../graphics/include/renderer/opengl/opengl_renderer.h"
#include "../../thirdparty/glew/include/GL/glew.h"
#ifdef TARGET_WINDOWS
#include "../../thirdparty/glew/include/GL/wglew.h"
#endif

#if defined(TARGET_WINDOWS)
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#else
//#error Choose OpenGL libraries for the project.
#endif

namespace sht {

	OpenGlApplication::OpenGlApplication()
	{

	}
	OpenGlApplication::~OpenGlApplication()
	{

	}
	bool OpenGlApplication::ShowStartupOptions()
	{
		return true;
	}
	bool OpenGlApplication::InitApi()
	{
#ifdef TARGET_WINDOWS
		const int kContextMajorVersion = 3;
		const int kContextMinorVersion = 3;

		PIXELFORMATDESCRIPTOR pfd =
		{
			sizeof(PIXELFORMATDESCRIPTOR),	// Size Of This Pixel Format Descriptor
			1,								// Version Number
			PFD_DRAW_TO_WINDOW |			// Format Must Support Window
			PFD_SUPPORT_OPENGL |			// Format Must Support OpenGL
			PFD_DOUBLEBUFFER,				// Must Support Double Buffering
			PFD_TYPE_RGBA,					// Request An RGBA Format
			color_bits_,			        // Select Our Color Depth
			0, 0, 0, 0, 0, 0,				// Color Bits Ignored
			0,								// No Alpha Buffer
			0,								// Shift Bit Ignored
			0,								// No Accumulation Buffer
			0, 0, 0, 0,						// Accumulation Bits Ignored
			depth_bits_,					// Z-Buffer (Depth Buffer)  
			0,								// No Stencil Buffer
			0,								// No Auxiliary Buffer
			PFD_MAIN_PLANE,					// Main Drawing Layer
			0,								// Reserved
			0, 0, 0							// Layer Masks Ignored
		};

		dc_ = GetDC(hwnd_);	// Grab A Device Context For This Window
		if (dc_ == 0)
		{
			return false;
		}

		int pixel_format = ChoosePixelFormat(dc_, &pfd);	// Find A Compatible Pixel Format
		if (pixel_format == 0)
		{
			ReleaseDC(hwnd_, dc_); dc_ = 0;
			return false;
		}

		if (SetPixelFormat(dc_, pixel_format, &pfd) == FALSE) // Try To Set The Pixel Format
		{
			ReleaseDC(hwnd_, dc_); dc_ = 0;
			return false;
		}

		HGLRC temp_rc = wglCreateContext(dc_);	// Try To Get A Rendering Context
		if (temp_rc == 0)
		{
			ReleaseDC(hwnd_, dc_); dc_ = 0;
			return false;
		}

		// Make The Rendering Context Our Current Rendering Context
		if (wglMakeCurrent(dc_, temp_rc) == FALSE)
		{
			// Failed
			wglDeleteContext(temp_rc);	rc_ = 0;
			ReleaseDC(hwnd_, dc_); dc_ = 0;
			return false;
		}

		// Initialize GLEW
		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			/* Problem: glewInit failed, something is seriously wrong. */
			fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
			return false;
		}

		int attribs[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, kContextMajorVersion,
			WGL_CONTEXT_MINOR_VERSION_ARB, kContextMinorVersion,
			WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			0
		};

		if ((wglewIsSupported("WGL_ARB_create_context")) &&
			(rc_ = wglCreateContextAttribsARB(dc_, 0, attribs)) &&
			(wglMakeCurrent(dc_, rc_)))
		{
			// Forward context has been successfully created
			wglDeleteContext(temp_rc);
		}
		else // failed to obtain forward context
		{
			// Use old context
			rc_ = temp_rc;
		}
#else
        //static_assert(false, "Do we need InitAPI under Mac ?");
#endif

		sht::graphics::OpenGlRenderer * renderer = new sht::graphics::OpenGlRenderer(width_, height_);
		renderer_ = renderer;

		return renderer->CheckFunctionalities();
	}
	void OpenGlApplication::DeinitApi()
	{
		delete renderer_;

#ifdef TARGET_WINDOWS
		if (dc_ != 0)
		{
			wglMakeCurrent(dc_, 0);
			if (rc_ != 0)
			{
				wglDeleteContext(rc_);
				rc_ = 0;
			}
			ReleaseDC(hwnd_, dc_);
			dc_ = 0;
		}
#endif
	}
	void OpenGlApplication::BeginFrame()
	{
		renderer_->Defaults();
	}
	void OpenGlApplication::EndFrame()
	{
#ifdef TARGET_WINDOWS
		SwapBuffers(dc_); // swap buffers
#endif
	}

} // namespace sht