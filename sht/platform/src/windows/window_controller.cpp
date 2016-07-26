#include "../../include/window_controller.h"
#include "../platform_inner.h"

#include "../window_struct.h"

#include "../../../application/application.h"

#include <stdio.h>	// for error logging
// OpenGL specific
#include "../../../thirdparty/glew/include/GL/glew.h"
#include "../../../thirdparty/glew/include/GL/wglew.h" // Windows only
#include <Windowsx.h> // for GET_X_LPARAM

#ifndef WM_MOUSEHWHEEL // It isn't defined on MinGW platform
#define WM_MOUSEHWHEEL 0x020E
#endif

namespace {
	PlatformWindow g_window;
	const char* kApplicationClassName = "ShtilleEngine";
}

//========================= Window Proc ==============================

// Translates Windows key modifiers to engine ones
static int TranslateModifiers(void)
{
	int modifier = 0;
	if (GetKeyState(VK_SHIFT) & (1 << 31))
		modifier |= sht::ModifierKey::kShift;
	if (GetKeyState(VK_CONTROL) & (1 << 31))
		modifier |= sht::ModifierKey::kControl;
	if (GetKeyState(VK_MENU) & (1 << 31))
		modifier |= sht::ModifierKey::kAlt;
	if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & (1 << 31))
		modifier |= sht::ModifierKey::kSuper;
	return modifier;
}

static sht::PublicKey TranslateKey(WPARAM wParam, LPARAM lParam)
{
	if (wParam == VK_CONTROL)
	{
		// The CTRL keys require special handling

		MSG next;
		DWORD time;

		// Is this an extended key (i.e. right key)?
		if (lParam & 0x01000000)
			return sht::PublicKey::kRightControl;

		// Here is a trick: "Alt Gr" sends LCTRL, then RALT. We only
		// want the RALT message, so we try to see if the next message
		// is a RALT message. In that case, this is a false LCTRL!
		time = GetMessageTime();

		if (PeekMessageW(&next, NULL, 0, 0, PM_NOREMOVE))
		{
			if (next.message == WM_KEYDOWN ||
				next.message == WM_SYSKEYDOWN ||
				next.message == WM_KEYUP ||
				next.message == WM_SYSKEYUP)
			{
				if (next.wParam == VK_MENU &&
					(next.lParam & 0x01000000) &&
					next.time == time)
				{
					// Next message is a RALT down message, which
					// means that this is not a proper LCTRL message
					return sht::PublicKey::kUnknown;
				}
			}
		}

		return sht::PublicKey::kLeftControl;
	}

	sht::Application * app = sht::Application::GetInstance();
	return app->keys().table(HIWORD(lParam) & 0x1FF);
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static POINT old_mouse_position = { 0 };
	static POINT mouse_position;

	sht::Application * app = sht::Application::GetInstance();
	switch (uMsg)
	{
	case WM_SYSCOMMAND:
		switch (wParam)
		{
		case SC_SCREENSAVE:		// Screensaver Trying To Start?
		case SC_MONITORPOWER:	// Monitor Trying To Enter Powersave?
			return 0;				// Prevent From Happening

		case SC_KEYMENU:        // Changing focus to system menu
			return 0;				// Prevent From Happening
		}
		break;

	case WM_CREATE:	// Window Creation
					// Initialize mouse position
		::GetCursorPos(&old_mouse_position);
		::ScreenToClient(hWnd, &old_mouse_position);
		return 0;

	case WM_CLOSE:	// Closing The Window
		DestroyWindow(hWnd);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_SETFOCUS: // Got focus
		return 0;
	case WM_KILLFOCUS: // Focus is lost, so minimize the window
		app->MakeWindowed();
		return 0;

	case WM_SIZE:	// Size Action Has Taken Place
		switch (wParam)
		{
		case SIZE_MINIMIZED:
			app->set_visible(false);
			return 0;

		case SIZE_MAXIMIZED:
			app->set_visible(true);
			return 0;

		case SIZE_RESTORED:
			//app->MakeFullscreen();
			app->set_visible(true);
			app->OnSize(LOWORD(lParam), HIWORD(lParam));
			return 0;
		}
		break;

	case WM_CHAR:
	{
		unsigned short code = static_cast<unsigned short>(wParam);
		if (sht::Keys::IsGoodChar(code))
			app->OnChar(code);
		return 0;
	}

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	{
		const sht::PublicKey translated_key = TranslateKey(wParam, lParam);
		if (translated_key == sht::PublicKey::kUnknown)
			break;
		const int modifiers = TranslateModifiers();

		app->keys().key_down(translated_key) = true;
		app->keys().modifiers() = modifiers;

		app->OnKeyDown(translated_key, modifiers);
		break;
	}

	case WM_KEYUP:
	case WM_SYSKEYUP:
	{
		const sht::PublicKey translated_key = TranslateKey(wParam, lParam);
		if (translated_key == sht::PublicKey::kUnknown)
			break;
		const int modifiers = TranslateModifiers();

		app->keys().key_down(translated_key) = false;
		app->keys().key_active(translated_key) = false;
		app->keys().modifiers() = 0;

		if (wParam == VK_SHIFT)
		{
			// Release both Shift keys on Shift up event, as only one event
			// is sent even if both keys are released
			app->OnKeyUp(sht::PublicKey::kLeftShift, modifiers);
			app->OnKeyUp(sht::PublicKey::kRightShift, modifiers);
		}
		else if (wParam == VK_SNAPSHOT)
		{
			// Key down is not reported for the print screen key
			app->OnKeyDown(sht::PublicKey::kPrintScreen, modifiers);
			app->OnKeyUp(sht::PublicKey::kPrintScreen, modifiers);
		}
		else
			app->OnKeyUp(translated_key, modifiers);

		break;
	}

	case WM_LBUTTONDOWN:
		app->mouse().button_down(sht::MouseButton::kLeft) = true;
		app->OnMouseDown(sht::MouseButton::kLeft, TranslateModifiers());
		return 0;
	case WM_LBUTTONUP:
		app->mouse().button_down(sht::MouseButton::kLeft) = false;
		app->OnMouseUp(sht::MouseButton::kLeft, TranslateModifiers());
		return 0;

	case WM_MBUTTONDOWN:
		app->mouse().button_down(sht::MouseButton::kMiddle) = true;
		app->OnMouseDown(sht::MouseButton::kMiddle, TranslateModifiers());
		return 0;
	case WM_MBUTTONUP:
		app->mouse().button_down(sht::MouseButton::kMiddle) = false;
		app->OnMouseUp(sht::MouseButton::kMiddle, TranslateModifiers());
		return 0;

	case WM_RBUTTONDOWN:
		app->mouse().button_down(sht::MouseButton::kRight) = true;
		app->OnMouseDown(sht::MouseButton::kRight, TranslateModifiers());
		return 0;
	case WM_RBUTTONUP:
		app->mouse().button_down(sht::MouseButton::kRight) = false;
		app->OnMouseUp(sht::MouseButton::kRight, TranslateModifiers());
		return 0;

	case WM_MOUSEMOVE:
		mouse_position.x = GET_X_LPARAM(lParam);
		mouse_position.y = GET_Y_LPARAM(lParam);
		if (mouse_position.x != old_mouse_position.x ||
			mouse_position.y != old_mouse_position.y)
		{
			app->mouse().delta_x() = static_cast<float>(mouse_position.x - old_mouse_position.x);
			app->mouse().delta_y() = static_cast<float>(mouse_position.y - old_mouse_position.y);

			RECT client_rect;
			::GetClientRect(hWnd, &client_rect);
			app->mouse().x() = static_cast<float>(mouse_position.x);
			app->mouse().y() = static_cast<float>(client_rect.bottom - mouse_position.y - 1);
			app->OnMouseMove();
			old_mouse_position = mouse_position;
		}
		return 0;

	case WM_MOUSEWHEEL:
		app->OnScroll(0.0f, (float)HIWORD(wParam) / (float)WHEEL_DELTA);
		return 0;
	case WM_MOUSEHWHEEL:
		app->OnScroll(-((float)HIWORD(wParam) / (float)WHEEL_DELTA), 0.0f);
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);	// Pass Unhandled Messages
}
//==================================================================

static void AdjustVideoSettings()
{
	sht::Application * app = sht::Application::GetInstance();

	g_window.current_state.style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	g_window.current_state.ex_style = WS_EX_APPWINDOW;

	g_window.current_state.rect.left = 0;
	g_window.current_state.rect.top = 0;
	g_window.current_state.rect.right = app->width();
	g_window.current_state.rect.bottom = app->height();

	bool windowed = true;
	if (app->fullscreen())	// Fullscreen Requested, Try Changing Video Modes
	{
		if (!PlatformWindowMakeFullscreen())
		{
			// Fullscreen Mode Failed.  Run In Windowed Mode Instead
			MessageBox(HWND_DESKTOP, TEXT("Mode Switch Failed.\nRunning In Windowed Mode."), TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
		}
		else // Otherwise (If Fullscreen Mode Was Successful)
		{
			windowed = false;
			g_window.current_state.style |= WS_POPUP;
			g_window.current_state.ex_style |= WS_EX_TOPMOST;
		}
	}
	if (windowed)
	{
		if (app->IsDecorated())
		{
			g_window.current_state.style |= WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

			if (app->IsResizable())
			{
				g_window.current_state.style |= WS_MAXIMIZEBOX | WS_SIZEBOX;
				g_window.current_state.ex_style |= WS_EX_WINDOWEDGE;
			}
		}
		else
		{
			g_window.current_state.style |= WS_POPUP;
		}
		// Adjust Window, Account For Window Borders
		AdjustWindowRectEx(&g_window.current_state.rect, g_window.current_state.style, 0, g_window.current_state.ex_style);
	}
}
static void RestoreVideoSettings()
{
	sht::Application * app = sht::Application::GetInstance();
	if (app->fullscreen())
	{
		ChangeDisplaySettingsA(NULL, 0);
	}
}

bool PlatformInit()
{
	g_window.need_quit = false;

	// Skip any upcoming error
	GetLastError();

	//	Some dual core systems have a problem where the different CPUs return different
	//	QueryPerformanceCounter values. So when this thread is schedule on the other CPU
	//	in a later frame, we could even get a negative frameTime. To solve this we force
	//	the main thread to always run on CPU 0.
	SetThreadAffinityMask(GetCurrentThread(), 1);

	// This is the way to change window icon manually:
	g_window.icon = LoadIcon(NULL, IDI_APPLICATION);
	if (g_window.icon == NULL)
	{
		MessageBox(HWND_DESKTOP, TEXT("Icon loading failed!"), TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	g_window.instance = GetModuleHandle(NULL);

	// Register A Window Class
	WNDCLASSEXA window_class;
	ZeroMemory(&window_class, sizeof(WNDCLASSEX));
	window_class.cbSize = sizeof(WNDCLASSEX);
	window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	window_class.lpfnWndProc = (WNDPROC)(WindowProc);
	window_class.hInstance = g_window.instance;
	window_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	window_class.hIcon = g_window.icon;
	window_class.hIconSm = g_window.icon;
	window_class.lpszClassName = kApplicationClassName;
	if (RegisterClassExA(&window_class) == 0)
	{
		DestroyIcon(g_window.icon);
		MessageBox(HWND_DESKTOP, TEXT("RegisterClassEx Failed!"), TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	AdjustVideoSettings();

	return true;
}
void PlatformTerminate()
{
	RestoreVideoSettings();

	UnregisterClassA(kApplicationClassName, g_window.instance);
	DestroyIcon(g_window.icon);
}
bool PlatformWindowCreate()
{
	sht::Application * app = sht::Application::GetInstance();

	// Create Window
	g_window.hwnd = CreateWindowExA(g_window.current_state.ex_style,			// Extended Style
		kApplicationClassName,													// Class Name
		app->GetTitle(),														// Window Title
		g_window.current_state.style,											// Window Style
		0, 0,																	// Window X,Y Position
		g_window.current_state.rect.right - g_window.current_state.rect.left,	// Window Width
		g_window.current_state.rect.bottom - g_window.current_state.rect.top,	// Window Height
		HWND_DESKTOP,															// Desktop Is Window's Parent
		0,																		// No Menu
		g_window.instance,														// Pass The Window Instance
		NULL);																	// pointer to window class

	return g_window.hwnd != NULL;
}
void PlatformWindowDestroy()
{
	DestroyWindow(g_window.hwnd);
}
bool PlatformNeedQuit()
{
	return g_window.need_quit;
}
void PlatformPollEvents()
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		// Handle or dispatch messages
		if (msg.message == WM_QUIT)
		{
			g_window.need_quit = true;
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}
void PlatformWindowMakeWindowed()
{
    ChangeDisplaySettings(NULL, 0); // restore display settings

	// Restore window state
	SetWindowLongPtr(g_window.hwnd, GWL_STYLE, g_window.old_state.style);
	SetWindowLongPtr(g_window.hwnd, GWL_EXSTYLE, g_window.old_state.ex_style);
	MoveWindow(g_window.hwnd, g_window.old_state.rect.left, g_window.old_state.rect.top,
		g_window.old_state.rect.right - g_window.old_state.rect.left,
		g_window.old_state.rect.bottom - g_window.old_state.rect.top, FALSE);
	ShowWindow(g_window.hwnd, SW_NORMAL);
}
bool PlatformWindowMakeFullscreen()
{
	sht::Application * app = sht::Application::GetInstance();
	
	// Store current window state:
	g_window.old_state.rect = g_window.current_state.rect;
	g_window.old_state.style = g_window.current_state.style;
	g_window.old_state.ex_style = g_window.current_state.ex_style;
	
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
		SetWindowLongPtr(g_window.hwnd, GWL_STYLE, g_window.old_state.style);
		SetWindowLongPtr(g_window.hwnd, GWL_EXSTYLE, g_window.old_state.ex_style);
		ShowWindow(g_window.hwnd, SW_NORMAL);
		return false;
	}

	SetWindowLongPtr(g_window.hwnd, GWL_STYLE, WS_POPUP);
	SetWindowLongPtr(g_window.hwnd, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_TOPMOST);
	MoveWindow(g_window.hwnd, 0, 0, app->width(), app->height(), FALSE);
	ShowWindow(g_window.hwnd, SW_NORMAL);
	return true;
}
void PlatformWindowCenter()
{
	RECT window_rect;
	::GetWindowRect(g_window.hwnd, &window_rect);
	int screen_width = ::GetSystemMetrics(SM_CXSCREEN);
	int screen_height = ::GetSystemMetrics(SM_CYSCREEN);
	int window_width = window_rect.right - window_rect.left;
	int window_height = window_rect.bottom - window_rect.top;
	window_rect.left = (screen_width - window_width)/2;
	window_rect.top = (screen_height - window_height)/2;
	::MoveWindow(g_window.hwnd, window_rect.left, window_rect.top, window_width, window_height, TRUE);
}
void PlatformWindowResize(int width, int height)
{
	RECT rect;
	POINT pos;
	pos.x = width;
	pos.y = height;
	::GetWindowRect(g_window.hwnd, &rect);
	::ClientToScreen(g_window.hwnd, &pos);
	::MoveWindow(g_window.hwnd, rect.left, rect.top, pos.x - rect.left, pos.y - rect.top, TRUE);
}
void PlatformWindowSetTitle(const char *title)
{
	SetWindowTextA(g_window.hwnd, title);
}
void PlatformWindowIconify()
{
	ShowWindow(g_window.hwnd, SW_MINIMIZE);
}
void PlatformWindowRestore()
{
	ShowWindow(g_window.hwnd, SW_NORMAL);
}
void PlatformWindowShow()
{
	ShowWindow(g_window.hwnd, SW_SHOW);
}
void PlatformWindowHide()
{
	ShowWindow(g_window.hwnd, SW_HIDE);
}
void PlatformWindowTerminate()
{
	PostMessage(g_window.hwnd, WM_CLOSE, 0, 0);
}
bool PlatformInitOpenGLContext(int color_bits, int depth_bits, int stencil_bits)
{
	sht::Application * app = sht::Application::GetInstance();

	const int kContextMajorVersion = 3;
	const int kContextMinorVersion = 3;

	static int msaa_pixel_format = 0;

	g_window.dc = GetDC(g_window.hwnd);	// Grab A Device Context For This Window
	if (g_window.dc == 0)
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

	int pixel_format = ChoosePixelFormat(g_window.dc, &pfd);	// Find A Compatible Pixel Format
	if (pixel_format == 0)
	{
		ReleaseDC(g_window.hwnd, g_window.dc); g_window.dc = 0;
		return false;
	}

	// Choose MSAA pixel format if needed
	if (msaa_pixel_format != 0)
		pixel_format = msaa_pixel_format;

	if (SetPixelFormat(g_window.dc, pixel_format, &pfd) == FALSE) // Try To Set The Pixel Format
	{
		ReleaseDC(g_window.hwnd, g_window.dc); g_window.dc = 0;
		return false;
	}

	HGLRC temp_rc = wglCreateContext(g_window.dc);	// Try To Get A Rendering Context
	if (temp_rc == 0)
	{
		ReleaseDC(g_window.hwnd, g_window.dc); g_window.dc = 0;
		return false;
	}

	// Make The Rendering Context Our Current Rendering Context
	if (wglMakeCurrent(g_window.dc, temp_rc) == FALSE)
	{
		// Failed
		wglDeleteContext(temp_rc);	g_window.rc = 0;
		ReleaseDC(g_window.hwnd, g_window.dc); g_window.dc = 0;
		return false;
	}

	// Initialize GLEW
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "glewInit failed: %s\n", glewGetErrorString(err));
		wglDeleteContext(temp_rc);	g_window.rc = 0;
		ReleaseDC(g_window.hwnd, g_window.dc); g_window.dc = 0;
		return false;
	}

	// Try to create a MSAA pixel format
	if (app->IsMultisample() && msaa_pixel_format == 0)
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

				if (wglChoosePixelFormatARB(g_window.dc, attribs, NULL, 1, &msaa_pixel_format, &num_formats) == TRUE && num_formats > 0)
				{
					// We cant's set pixel format twice, so we have to recreate a window *?*
					wglDeleteContext(temp_rc);	g_window.rc = 0;
					ReleaseDC(g_window.hwnd, g_window.dc); g_window.dc = 0;
					PlatformWindowDestroy();
					// Don't forget do dispatch any upcoming messages, such as WM_QUIT
					MSG	msg;
					while (GetMessage(&msg, NULL, 0, 0))
					{
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
					if (PlatformWindowCreate())
					{
						PlatformWindowCenter();
						// Call this function again with msaa pixel format already known
						return PlatformInitOpenGLContext(color_bits, depth_bits, stencil_bits);
					}
					else
						return false;
				}
				else
					msaa_pixel_format = 0;

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
		(g_window.rc = wglCreateContextAttribsARB(g_window.dc, 0, attribs)) &&
		(wglMakeCurrent(g_window.dc, g_window.rc)))
	{
		// Forward context has been successfully created
		wglDeleteContext(temp_rc);
	}
	else // failed to obtain forward context
	{
		// Use old context
		g_window.rc = temp_rc;
	}

	return true;
}
void PlatformDeinitOpenGLContext()
{
	if (g_window.dc != 0)
	{
		wglMakeCurrent(g_window.dc, 0);
		if (g_window.rc != 0)
		{
			wglDeleteContext(g_window.rc);
			g_window.rc = 0;
		}
		ReleaseDC(g_window.hwnd, g_window.dc);
		g_window.dc = 0;
	}
}
void PlatformSwapBuffers()
{
	SwapBuffers(g_window.dc);
}
void PlatformMakeContextCurrent()
{
	wglMakeCurrent(g_window.dc, g_window.rc);
}
void PlatformSwapInterval(int interval)
{
	if (WGLEW_EXT_swap_control)
		wglSwapIntervalEXT(interval);
}
void PlatformSetCursorPos(float x, float y)
{
	RECT winrect;
	POINT pos;
	::GetClientRect(g_window.hwnd, &winrect);
	pos.x = static_cast<LONG>(x);
	pos.y = static_cast<LONG>(winrect.bottom - y - 1);
	::ClientToScreen(g_window.hwnd, &pos);
	::SetCursorPos((int)pos.x, (int)pos.y);
}
void PlatformGetCursorPos(float& x, float& y)
{
	RECT winrect;
	POINT pos;
	::GetCursorPos(&pos);
	::GetClientRect(g_window.hwnd, &winrect);
	::ScreenToClient(g_window.hwnd, &pos);
	x = static_cast<float>(pos.x); // [0, w]
	y = static_cast<float>(winrect.bottom - pos.y - 1); // [0, h]
}
void PlatformMouseToCenter()
{
	RECT winrect;
	POINT pos;
	::GetClientRect(g_window.hwnd, &winrect);
	pos.x = (winrect.right - winrect.left) / 2;
	pos.y = (winrect.bottom - winrect.top) / 2;
	::ClientToScreen(g_window.hwnd, &pos);
	::SetCursorPos((int)(pos.x), (int)(pos.y));
}
void PlatformShowCursor()
{
	::ShowCursor(TRUE);
}
void PlatformHideCursor()
{
	::ShowCursor(FALSE);
}
void PlatformSetClipboardText(const char *text)
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
std::string PlatformGetClipboardText()
{
	OpenClipboard(NULL);
	HANDLE pText = GetClipboardData(CF_TEXT);
	CloseClipboard();
	LPVOID text = GlobalLock(pText);
	std::string string = (char*)text;
	GlobalUnlock(pText);
	return string;
}
void PlatformChangeDirectoryToResources()
{
	char buffer[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, buffer);
	lstrcat(buffer, TEXT("\\.."));
	SetCurrentDirectory(buffer);
}