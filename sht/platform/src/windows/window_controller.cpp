#include "../window_controller_interface.h"
#include "../window_struct.h"
#include "../../../common/platform.h"
#include "../../../application/application.h"
#include <stdio.h>	// for error logging
// OpenGL specific
#include "../../../thirdparty/glew/include/GL/glew.h"
#include "../../../thirdparty/glew/include/GL/wglew.h" // Windows only

static struct WindowState {
	RECT window_rect;
	LONG_PTR style;
	LONG_PTR ex_style;
} old_window;

void PlatformWindowMakeWindowedImpl(void *instance)
{
	PlatformWindow * window = reinterpret_cast<PlatformWindow*>(instance);
    ChangeDisplaySettings(NULL, 0); // restore display settings

	// Restore window state
	SetWindowLongPtr(window->hwnd, GWL_STYLE, old_window.style);
	SetWindowLongPtr(window->hwnd, GWL_EXSTYLE, old_window.ex_style);
	MoveWindow(window->hwnd, old_window.window_rect.left,
							 old_window.window_rect.top, 
							 old_window.window_rect.right - old_window.window_rect.left,
							 old_window.window_rect.bottom - old_window.window_rect.top, FALSE);
	ShowWindow(window->hwnd, SW_NORMAL);
}
void PlatformWindowMakeFullscreenImpl(void *instance)
{
	PlatformWindow * window = reinterpret_cast<PlatformWindow*>(instance);
	sht::Application * app = sht::Application::GetInstance();
	
	// Store current window state:
	GetWindowRect(window->hwnd, &old_window.window_rect);
	old_window.style = GetWindowLongPtr(window->hwnd, GWL_STYLE);
	old_window.ex_style = GetWindowLongPtr(window->hwnd, GWL_EXSTYLE);
	
    DEVMODE dmScreenSettings;
	ZeroMemory(&dmScreenSettings, sizeof(DEVMODE));
	dmScreenSettings.dmSize = sizeof(DEVMODE);
	dmScreenSettings.dmPelsWidth = app->width();
	dmScreenSettings.dmPelsHeight = app->height();
	dmScreenSettings.dmBitsPerPel = app->color_bits();
	dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
	{
		// Still windowed
		// fullscreen_ = false; // TODO: make function bool
		SetWindowLongPtr(window->hwnd, GWL_STYLE, old_window.style);
		SetWindowLongPtr(window->hwnd, GWL_EXSTYLE, old_window.ex_style);
		ShowWindow(window->hwnd, SW_NORMAL);
		return;
	}

	SetWindowLongPtr(window->hwnd, GWL_STYLE, WS_POPUP);
	SetWindowLongPtr(window->hwnd, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_TOPMOST);
	MoveWindow(window->hwnd, 0, 0, app->width(), app->height(), FALSE);
	ShowWindow(window->hwnd, SW_NORMAL);
}
void PlatformWindowCenterImpl(void *instance)
{
	PlatformWindow * window = reinterpret_cast<PlatformWindow*>(instance);
	RECT window_rect;
	::GetWindowRect(window->hwnd, &window_rect);
	int screen_width = ::GetSystemMetrics(SM_CXSCREEN);
	int screen_height = ::GetSystemMetrics(SM_CYSCREEN);
	int window_width = window_rect.right - window_rect.left;
	int window_height = window_rect.bottom - window_rect.top;
	window_rect.left = (screen_width - window_width)/2;
	window_rect.top = (screen_height - window_height)/2;
	::MoveWindow(window->hwnd, window_rect.left, window_rect.top, window_width, window_height, TRUE);
}
void PlatformWindowResizeImpl(void *instance, int width, int height)
{
	PlatformWindow * window = reinterpret_cast<PlatformWindow*>(instance);
	RECT rect;
	POINT pos;
	pos.x = width;
	pos.y = height;
	::GetWindowRect(window->hwnd, &rect);
	::ClientToScreen(window->hwnd, &pos);
	::MoveWindow(window->hwnd, rect.left, rect.top, pos.x - rect.left, pos.y - rect.top, TRUE);
}
void PlatformWindowSetTitleImpl(void *instance, const char *title)
{
	PlatformWindow * window = reinterpret_cast<PlatformWindow*>(instance);
	SetWindowTextA(window->hwnd, title);
}
void PlatformWindowIconifyImpl(void *instance)
{
	PlatformWindow * window = reinterpret_cast<PlatformWindow*>(instance);
	ShowWindow(window->hwnd, SW_MINIMIZE);
}
void PlatformWindowRestoreImpl(void *instance)
{
	PlatformWindow * window = reinterpret_cast<PlatformWindow*>(instance);
	ShowWindow(window->hwnd, SW_NORMAL);
}
void PlatformWindowShowImpl(void *instance)
{
	PlatformWindow * window = reinterpret_cast<PlatformWindow*>(instance);
	ShowWindow(window->hwnd, SW_SHOW);
}
void PlatformWindowHideImpl(void *instance)
{
	PlatformWindow * window = reinterpret_cast<PlatformWindow*>(instance);
	ShowWindow(window->hwnd, SW_HIDE);
}
void PlatformWindowTerminateImpl(void *instance)
{
	PlatformWindow * window = reinterpret_cast<PlatformWindow*>(instance);
	PostMessage(window->hwnd, WM_CLOSE, 0, 0);
}
bool PlatformInitOpenGLContextImpl(void *instance, int color_bits, int depth_bits, int stencil_bits)
{
	PlatformWindow * window = reinterpret_cast<PlatformWindow*>(instance);
	sht::Application * app = sht::Application::GetInstance();

	const int kContextMajorVersion = 3;
	const int kContextMinorVersion = 3;

	window->dc = GetDC(window->hwnd);	// Grab A Device Context For This Window
	if (window->dc == 0)
	{
		return false;
	}

	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),	// Size Of This Pixel Format Descriptor
		1,								// Version Number
		PFD_DRAW_TO_WINDOW |			// Format Must Support Window
		PFD_SUPPORT_OPENGL |			// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,				// Must Support Double Buffering
		PFD_TYPE_RGBA,					// Request An RGBA Format
		(BYTE)color_bits,				// Select Our Color Depth
		0, 0, 0, 0, 0, 0,				// Color Bits Ignored
		0,								// No Alpha Buffer
		0,								// Shift Bit Ignored
		0,								// No Accumulation Buffer
		0, 0, 0, 0,						// Accumulation Bits Ignored
		(BYTE)depth_bits,				// Z-Buffer (Depth Buffer)
		(BYTE)stencil_bits,				// Stencil Buffer
		0,								// No Auxiliary Buffer
		PFD_MAIN_PLANE,					// Main Drawing Layer
		0,								// Reserved
		0, 0, 0							// Layer Masks Ignored
	};

	int pixel_format = ChoosePixelFormat(window->dc, &pfd);	// Find A Compatible Pixel Format
	if (pixel_format == 0)
	{
		ReleaseDC(window->hwnd, window->dc); window->dc = 0;
		return false;
	}

	if (app->msaa_pixel_format != 0)
		pixel_format = app->msaa_pixel_format;

	if (SetPixelFormat(window->dc, pixel_format, &pfd) == FALSE) // Try To Set The Pixel Format
	{
		ReleaseDC(window->hwnd, window->dc); window->dc = 0;
		return false;
	}

	HGLRC temp_rc = wglCreateContext(window->dc);	// Try To Get A Rendering Context
	if (temp_rc == 0)
	{
		ReleaseDC(window->hwnd, window->dc); window->dc = 0;
		return false;
	}

	// Make The Rendering Context Our Current Rendering Context
	if (wglMakeCurrent(window->dc, temp_rc) == FALSE)
	{
		// Failed
		wglDeleteContext(temp_rc);	window->rc = 0;
		ReleaseDC(window->hwnd, window->dc); window->dc = 0;
		return false;
	}

	// Initialize GLEW
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "glewInit failed: %s\n", glewGetErrorString(err));
		wglDeleteContext(temp_rc);	window->rc = 0;
		ReleaseDC(window->hwnd, window->dc); window->dc = 0;
		return false;
	}

	// Try to create a MSAA pixel format
	if (app->IsMultisample() && app->msaa_pixel_format == 0)
	{
		if (GLEW_ARB_multisample && WGLEW_ARB_pixel_format)
		{
			int samples = 4; // = msaa_samples

			while (samples > 0)
			{
				UINT num_formats = 0;

				int attribs[] =
				{
					WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
					WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
					WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
					WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
					WGL_COLOR_BITS_ARB, color_bits,
					WGL_DEPTH_BITS_ARB, depth_bits,
					WGL_STENCIL_BITS_ARB, stencil_bits,
					WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
					WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
					WGL_SAMPLES_ARB, samples,
					0
				};

				int msaa_pixel_format = 0;
				if (wglChoosePixelFormatARB(window->dc, attribs, NULL, 1, &msaa_pixel_format, &num_formats) == TRUE && num_formats > 0)
				{
					// We cant's set pixel format twice, so we have to recreate a window *?*
					app->msaa_pixel_format = msaa_pixel_format;
					wglDeleteContext(temp_rc);	window->rc = 0;
					ReleaseDC(window->hwnd, window->dc); window->dc = 0;
					return false;
				}

				--samples;
			}
			fprintf(stderr, "Error: failed to find any compatible MSAA format\n");
		}
		else
		{
			fprintf(stderr, "Error: WGL_ARB_multisample isn't supported, using standart context\n");
		}
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
		(window->rc = wglCreateContextAttribsARB(window->dc, 0, attribs)) &&
		(wglMakeCurrent(window->dc, window->rc)))
	{
		// Forward context has been successfully created
		wglDeleteContext(temp_rc);
	}
	else // failed to obtain forward context
	{
		// Use old context
		window->rc = temp_rc;
	}

	if (app->IsBenchmark() && WGLEW_EXT_swap_control)
	{
		wglSwapIntervalEXT(0);
	}

	return true;
}
void PlatformDeinitOpenGLContextImpl(void *instance)
{
	PlatformWindow * window = reinterpret_cast<PlatformWindow*>(instance);
	if (window->dc != 0)
	{
		wglMakeCurrent(window->dc, 0);
		if (window->rc != 0)
		{
			wglDeleteContext(window->rc);
			window->rc = 0;
		}
		ReleaseDC(window->hwnd, window->dc);
		window->dc = 0;
	}
}
void PlatformSwapBuffersImpl(void *instance)
{
	PlatformWindow * window = reinterpret_cast<PlatformWindow*>(instance);
	SwapBuffers(window->dc);
}
void PlatformSetCursorPosImpl(void *instance, float x, float y)
{
	PlatformWindow * window = reinterpret_cast<PlatformWindow*>(instance);
	RECT winrect;
	POINT pos;
	::GetClientRect(window->hwnd, &winrect);
	pos.x = static_cast<LONG>(x);
	pos.y = static_cast<LONG>(winrect.bottom - y - 1);
	::ClientToScreen(window->hwnd, &pos);
	::SetCursorPos((int)pos.x, (int)pos.y);
}
void PlatformGetCursorPosImpl(void *instance, float& x, float& y)
{
	PlatformWindow * window = reinterpret_cast<PlatformWindow*>(instance);
	RECT winrect;
	POINT pos;
	::GetCursorPos(&pos);
	::GetClientRect(window->hwnd, &winrect);
	::ScreenToClient(window->hwnd, &pos);
	x = static_cast<float>(pos.x); // [0, w]
	y = static_cast<float>(winrect.bottom - pos.y - 1); // [0, h]
}
void PlatformMouseToCenterImpl(void *instance)
{
	PlatformWindow * window = reinterpret_cast<PlatformWindow*>(instance);
	RECT winrect;
	POINT pos;
	::GetClientRect(window->hwnd, &winrect);
	pos.x = (winrect.right - winrect.left) / 2;
	pos.y = (winrect.bottom - winrect.top) / 2;
	::ClientToScreen(window->hwnd, &pos);
	::SetCursorPos((int)(pos.x), (int)(pos.y));
}
void PlatformShowCursorImpl(void *instance)
{
	::ShowCursor(TRUE);
}
void PlatformHideCursorImpl(void *instance)
{
	::ShowCursor(FALSE);
}
void PlatformSetClipboardTextImpl(void *instance, const char *text)
{
	size_t len = strlen(text);
	HGLOBAL x = GlobalAlloc(GMEM_DDESHARE|GMEM_MOVEABLE, (len+1)*sizeof(char));
	char* y = (char*)GlobalLock(x);
	strcpy(y, text);
	y[len] = '\0';
	GlobalUnlock(x);
	OpenClipboard(NULL);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, x);
	CloseClipboard();
}
std::string PlatformGetClipboardTextImpl(void *instance)
{
	OpenClipboard(NULL);
	HANDLE pText = GetClipboardData(CF_TEXT);
	CloseClipboard();
	LPVOID text = GlobalLock(pText);
	std::string string = (char*)text;
	GlobalUnlock(pText);
	return string;
}
void PlatformChangeDirectoryToResourcesImpl()
{
	char buffer[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, buffer);
	lstrcat(buffer, TEXT("\\.."));
	SetCurrentDirectory(buffer);
}