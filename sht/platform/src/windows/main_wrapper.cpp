#include "../../include/main_wrapper.h"
#include "../window_struct.h"
#include "../../../common/platform.h"
#include "../../../application/application.h"
#include "../../../system/include/update_timer.h"

void * g_window_controller = nullptr;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
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
		return 0;

	case WM_CLOSE:	// Closing The Window
		DestroyWindow(hWnd);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_KILLFOCUS: // Focus is lost, so minimize the window
		app->MakeWindowed();
		//ShowWindow(hWnd, SW_MINIMIZE);
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
		app->keys().key_down(app->keys().table(wParam)) = true;
		return 0;

	case WM_KEYUP:
		app->keys().key_down(app->keys().table(wParam)) = false;
		app->keys().key_active(app->keys().table(wParam)) = false;
		return 0;

	case WM_LBUTTONDOWN:
		app->OnMouseDown(sht::MouseButton::kLeft, 0);
		break;
	case WM_LBUTTONUP:
		app->OnMouseUp(sht::MouseButton::kLeft, 0);
		break;

	case WM_MBUTTONDOWN:
		app->OnMouseDown(sht::MouseButton::kMiddle, 0);
		break;
	case WM_MBUTTONUP:
		app->OnMouseUp(sht::MouseButton::kMiddle, 0);
		break;

	case WM_RBUTTONDOWN:
		app->OnMouseDown(sht::MouseButton::kRight, 0);
		break;
	case WM_RBUTTONUP:
		app->OnMouseUp(sht::MouseButton::kRight, 0);
		break;

	case WM_MOUSEMOVE:
		app->OnMouseMove();
		break;

	case WM_MOUSEWHEEL:
		app->OnScroll(0.0f, (float)GET_WHEEL_DELTA_WPARAM(wParam));
		break;
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