#include "../window_controller_interface.h"
#include "../window_struct.h"
#include "../../../common/platform.h"
#include "../../../application/application.h"
// OpenGL specific
#include "../../../thirdparty/glew/include/GL/glew.h"
#include "../../../thirdparty/glew/include/GL/wglew.h" // Windows only

void PlatformWindowMakeWindowedImpl(void *instance)
{
	PlatformWindow * window = reinterpret_cast<PlatformWindow*>(instance);
    ChangeDisplaySettings(NULL, 0); // restore display settings

	SetWindowLongPtr(window->hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
	SetWindowLongPtr(window->hwnd, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_DLGMODALFRAME);
	ShowWindow(window->hwnd, SW_NORMAL);
}
void PlatformWindowMakeFullscreenImpl(void *instance)
{
	PlatformWindow * window = reinterpret_cast<PlatformWindow*>(instance);
	sht::Application * app = sht::Application::GetInstance();
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
		SetWindowLongPtr(window->hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		SetWindowLongPtr(window->hwnd, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_DLGMODALFRAME);
		ShowWindow(window->hwnd, SW_NORMAL);
		return;
	}

	SetWindowLongPtr(window->hwnd, GWL_STYLE, WS_POPUP);
	SetWindowLongPtr(window->hwnd, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_TOPMOST);
	ShowWindow(window->hwnd, SW_NORMAL);
}
void PlatformWindowResizeImpl(void *instance, int width, int height)
{
	PlatformWindow * window = reinterpret_cast<PlatformWindow*>(instance);
	MoveWindow(window->hwnd, 0, 0, width, height, TRUE);
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
bool PlatformInitOpenGLContextImpl(void *instance, int color_bits, int depth_bits)
{
	PlatformWindow * window = reinterpret_cast<PlatformWindow*>(instance);

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
		color_bits,						// Select Our Color Depth
		0, 0, 0, 0, 0, 0,				// Color Bits Ignored
		0,								// No Alpha Buffer
		0,								// Shift Bit Ignored
		0,								// No Accumulation Buffer
		0, 0, 0, 0,						// Accumulation Bits Ignored
		depth_bits,						// Z-Buffer (Depth Buffer)  
		0,								// No Stencil Buffer
		0,								// No Auxiliary Buffer
		PFD_MAIN_PLANE,					// Main Drawing Layer
		0,								// Reserved
		0, 0, 0							// Layer Masks Ignored
	};

	window->dc = GetDC(window->hwnd);	// Grab A Device Context For This Window
	if (window->dc == 0)
	{
		return false;
	}

	int pixel_format = ChoosePixelFormat(window->dc, &pfd);	// Find A Compatible Pixel Format
	if (pixel_format == 0)
	{
		ReleaseDC(window->hwnd, window->dc); window->dc = 0;
		return false;
	}

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
void PlatformSetCursorPosImpl(void *instance, int x, int y)
{
	PlatformWindow * window = reinterpret_cast<PlatformWindow*>(instance);
	RECT winrect;
	GetWindowRect(window->hwnd, &winrect);
	SetCursorPos((int)winrect.left + x, (int)winrect.bottom - y);
}
void PlatformGetCursorPosImpl(void *instance, int& x, int& y)
{
	PlatformWindow * window = reinterpret_cast<PlatformWindow*>(instance);
	RECT winrect;
	GetWindowRect(window->hwnd, &winrect);
	POINT p;
	GetCursorPos(&p);
	x = p.x - winrect.left; // [0, w]
	y = winrect.bottom - p.y; // [0, h]
}
void PlatformMouseToCenterImpl(void *instance)
{
	PlatformWindow * window = reinterpret_cast<PlatformWindow*>(instance);
	RECT winrect;
	POINT p;
	GetWindowRect(window->hwnd, &winrect);
	p.x = (winrect.bottom - winrect.top) / 2;
	p.y = (winrect.right - winrect.left) / 2;
	SetCursorPos((int)(winrect.left + p.x), (int)(winrect.bottom - p.y));
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
	lstrcat(buffer, TEXT("\\..\\TestProject"));
	SetCurrentDirectory(buffer);
}