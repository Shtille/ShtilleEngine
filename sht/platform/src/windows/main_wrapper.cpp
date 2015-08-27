#include "../../include/main_wrapper.h"
#include "../window_struct.h"
#include "../../../common/platform.h"
#include "../../../application/application.h"
#include "../../../system/include/update_timer.h"
#include <Windowsx.h> // for GET_X_LPARAM

#ifndef WM_MOUSEHWHEEL
# define WM_MOUSEHWHEEL 0x020E
#endif

void * g_window_controller = nullptr;

static bool g_can_handle_mouse_move_event = true; // Hack to do not let mouse move event to overflow messages queue

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
	static POINT mouse_position;
	static POINT old_mouse_position;
	
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
		//if (app->keys().key_queue_size < 64)
		//	app->keys().key_queue[app->keys().key_queue_size++] = (char)wParam;
		return 0;

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
		if (g_can_handle_mouse_move_event)
		{
			g_can_handle_mouse_move_event = false;
			
			app->mouse().delta_x() = static_cast<float>(mouse_position.x - old_mouse_position.x);
			app->mouse().delta_y() = static_cast<float>(mouse_position.y - old_mouse_position.y);
			
			RECT client_rect;
			::GetClientRect(hWnd, &client_rect);
			app->mouse().x() = static_cast<float>(mouse_position.x);
			app->mouse().y() = static_cast<float>(client_rect.bottom - mouse_position.y - 1);
			app->OnMouseMove();
		}
		old_mouse_position = mouse_position;
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

int MainWrapper(int argc, const char** argv)
{
	sht::Application * app = sht::Application::GetInstance();

	// Window creation
    const char* app_class_name = "ShtilleEngine";
    
    GetLastError(); // skip any upcoming error
    
	HINSTANCE instance = GetModuleHandle(NULL);

	// Register window class
	// This is the way to change window icon manually:
	HICON icon = LoadIcon(NULL, IDI_APPLICATION);
	if (icon == NULL)
	{
		MessageBox(HWND_DESKTOP, TEXT("Icon loading failed!"), TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
		return 1;
	}

	// Register A Window Class
	WNDCLASSEXA windowClass;
	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = (WNDPROC)(WindowProc);
	windowClass.hInstance = instance;
	windowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hIcon = icon;
	windowClass.hIconSm = icon;
	windowClass.lpszClassName = app_class_name;
	if (RegisterClassExA(&windowClass) == 0)
	{
		DestroyIcon(icon);
		MessageBox(HWND_DESKTOP, TEXT("RegisterClassEx Failed!"), TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
		return 1;
	}

	DWORD windowStyle = WS_OVERLAPPEDWINDOW;
	DWORD windowExtendedStyle = WS_EX_APPWINDOW | WS_EX_DLGMODALFRAME;

	RECT winRect = { 0, 0, app->width(), app->height() };	// Define Our Window Coordinates

	if (app->fullscreen())	// Fullscreen Requested, Try Changing Video Modes
	{
		if (!app->MakeFullscreen())
		{
			// Fullscreen Mode Failed.  Run In Windowed Mode Instead
			MessageBox(HWND_DESKTOP, TEXT("Mode Switch Failed.\nRunning In Windowed Mode."), TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
		}
		else	// Otherwise (If Fullscreen Mode Was Successful)
		{
			windowStyle = WS_POPUP;
			windowExtendedStyle |= WS_EX_TOPMOST;
		}
	}
	else
	{
		// Adjust Window, Account For Window Borders
		AdjustWindowRectEx(&winRect, windowStyle, 0, windowExtendedStyle);
	}

	// Create Window
	PlatformWindow window;
	window.hwnd = CreateWindowExA(windowExtendedStyle,	// Extended Style
		app_class_name,	        // Class Name
		app->GetTitle(),		// Window Title
		windowStyle,			// Window Style
		0, 0,				// Window X,Y Position
		winRect.right - winRect.left,	// Window Width
		winRect.bottom - winRect.top,	// Window Height
		HWND_DESKTOP,	// Desktop Is Window's Parent
		0,			// No Menu
		instance,  // Pass The Window Instance
		NULL); // pointer to window class
	if (window.hwnd == NULL)
	{
		// Window creation failed
		UnregisterClassA(app_class_name, instance);
		DestroyIcon(icon);
		return 1;
	}
	g_window_controller = reinterpret_cast<void*>(&window);
	app->Center();

	if (app->InitApi())
	{
		if (app->Load())
		{
			// Show window
			ShowWindow(window.hwnd, SW_NORMAL);
			app->set_visible(true);

			//	Some dual core systems have a problem where the different CPUs return different
			//	QueryPerformanceCounter values. So when this thread is schedule on the other CPU
			//	in a later frame, we could even get a negative frameTime. To solve this we force
			//	the main thread to always run on CPU 0.
			SetThreadAffinityMask(GetCurrentThread(), 1);

			// Start time
            sht::system::UpdateTimer update_timer;
			update_timer.Start();

			// program main loop
			MSG	msg;
			bool bQuit = false;
			while (!bQuit)
			{
				// calculate dt
				app->SetFrameTime(update_timer.GetElapsedTime());

				// check for messages
				while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					// handle or dispatch messages
					if (msg.message == WM_QUIT)
					{
						bQuit = true;
					}
					else
					{
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
				}
				if (bQuit) break;
				g_can_handle_mouse_move_event = true;

				app->Update();

				app->BeginFrame();
				app->Render();
				app->EndFrame();
			}
		}
        app->Unload(); // delete allocated objects (may be allocated partially)
        app->DeinitApi();
	}
	else
	{
		MessageBox(HWND_DESKTOP, TEXT("Failed to init API!"), TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
		DestroyWindow(window.hwnd);
	}

	if (app->fullscreen())
	{
		ChangeDisplaySettings(NULL, 0);
	}

	UnregisterClassA(app_class_name, instance);
	DestroyIcon(icon);

	return 0;
}