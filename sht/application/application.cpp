#include "application.h"
#include <memory.h>
#include "../graphics/include/renderer/renderer.h"
#include "../system/include/memory_leaks.h"
#include "../system/include/time.h"
#include "../system/include/stream/file_stream.h"
#include <cstdlib>
#if defined(TARGET_MAC)
#include <AGL/AGL.h>
#endif
#include <algorithm> // for std::max
#ifdef TARGET_WINDOWS
#undef max
#endif

namespace sht {

	Application *Application::app_ = nullptr;

	Application::Application()
	{
#ifdef TARGET_WINDOWS
		hwnd_ = nullptr;
#endif
		visible_ = false;
		fullscreen_ = false;
		color_bits_ = 32;
		depth_bits_ = 32;

		memset(&keys_, 0, sizeof(keys_));

		time_ = 0.0f;
		frame_time_ = 0.0f;

		fps_counter_time_ = 0.0f;
		fps_counter_count_ = 0.0f;
		need_take_screenshot_ = false;
	}
	Application::~Application()
	{

	}
#ifdef TARGET_WINDOWS
	LRESULT CALLBACK Application::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
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
			app_->Unload();
			app_->DeinitApi();
			DestroyWindow(hWnd);
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		case WM_KILLFOCUS: // Focus is lost, so minimize the window
			app_->MakeWindowed();
			//ShowWindow(hWnd, SW_MINIMIZE);
			return 0;

		case WM_SIZE:	// Size Action Has Taken Place
			switch (wParam)
			{
			case SIZE_MINIMIZED:
				app_->set_visible(false);
				return 0;

			case SIZE_MAXIMIZED:
				app_->set_visible(true);
				return 0;

			case SIZE_RESTORED:
				app_->MakeFullscreen();
				app_->set_visible(true);
				app_->OnSize(LOWORD(lParam), HIWORD(lParam));
				return 0;
			}
			break;

		case WM_CHAR:
			if (app_->keys().key_queue_size < 64)
				app_->keys().key_queue[app_->keys().key_queue_size++] = (char)wParam;
			return 0;

		case WM_KEYDOWN:
			app_->keys().key_down[wParam] = true;
			return 0;

		case WM_KEYUP:
			app_->keys().key_down[wParam] = false;
			app_->keys().key_active[wParam] = false;
			return 0;

		case WM_LBUTTONDOWN:
			app_->OnLButtonDown();
			break;
		case WM_LBUTTONUP:
			app_->OnLButtonUp();
			break;

		case WM_MBUTTONDOWN:
			app_->OnMButtonDown();
			break;
		case WM_MBUTTONUP:
			app_->OnMButtonUp();
			break;

		case WM_RBUTTONDOWN:
			app_->OnRButtonDown();
			break;
		case WM_RBUTTONUP:
			app_->OnRButtonUp();
			break;

		case WM_MOUSEMOVE:
			app_->OnMouseMove();
			break;

		case WM_MOUSEWHEEL:
			app_->OnMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam));
			break;
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);	// Pass Unhandled Messages
	}
	bool Application::RegisterWindowClass(HINSTANCE inst, const char* appclassname)
	{
		// This is the way to change window icon manually:
		app_->icon_ = LoadIcon(NULL, MAKEINTRESOURCE(IDI_WINLOGO));
		if (app_->icon_ == nullptr)
		{
			MessageBox(HWND_DESKTOP, TEXT("Icon loading failed!"), TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
			return false;
		}

		// Register A Window Class
		WNDCLASSEXA windowClass;
		ZeroMemory(&windowClass, sizeof(WNDCLASSEX));
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		windowClass.lpfnWndProc = (WNDPROC)(WindowProc);
		windowClass.hInstance = inst;
		windowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		windowClass.hIcon = app_->icon_;
		windowClass.hIconSm = app_->icon_;
		windowClass.lpszClassName = appclassname;
		if (RegisterClassExA(&windowClass) == 0)
		{
			DestroyIcon(app_->icon_);
			MessageBox(HWND_DESKTOP, TEXT("RegisterClassEx Failed!"), TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
			return false;
		}
		return true;
	}
	void Application::WindowsErrorCheck()
	{
#ifdef _DEBUG
		DWORD error = GetLastError();
		if (error == 0)
			return;
		TCHAR buf[256];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 256, NULL);
		MessageBox(HWND_DESKTOP, buf, TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
#endif
	}
#endif
	void Application::Run()
	{
		app_ = this;

		// Enable automatic memory leaks checking
		sht::system::EnableMemoryLeaksChecking();

		sht::system::UpdateTimer update_timer;

		// Prestart initialization
		if (!app_->ShowStartupOptions())
		{
			return;
		}
		if (!app_->PreStartInit())
		{
			return;
		}

		sht::system::FileStream ini_file;
		if (ini_file.Open(app_->GetIniFilePath(), sht::system::StreamAccess::kReadText))
		{
			char temp[10]; int w, h; bool fullscr;

			ini_file.ScanString("%i x %i", &w, &h);
			ini_file.ScanString("%s", temp, _countof(temp));
			ini_file.Close();

			fullscr = strcmp(temp, "TRUE") == 0;

			app_->InitWindowSize(w, h, fullscr);
		}
		else
		{
			app_->InitWindowSize(1024, 768, false);
		}

#if defined(TARGET_WINDOWS)
        const char* app_class_name = "ShtilleEngine";
        
        GetLastError(); // skip any upcoming error
        
		HINSTANCE instance = GetModuleHandle(NULL);
		if (!RegisterWindowClass(instance, app_class_name))
		{
			MessageBox(HWND_DESKTOP, TEXT("Failed to register a window class!"), TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		DWORD windowStyle = WS_OVERLAPPEDWINDOW;
		DWORD windowExtendedStyle = WS_EX_APPWINDOW | WS_EX_DLGMODALFRAME;

		RECT winRect = { 0, 0, app_->width(), app_->height() };	// Define Our Window Coordinates

		if (app_->fullscreen())	// Fullscreen Requested, Try Changing Video Modes
		{
			if (!app_->MakeFullscreen())
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
		HWND hwnd = CreateWindowExA(windowExtendedStyle,	// Extended Style
			app_class_name,	        // Class Name
			app_->GetTitle(),		// Window Title
			windowStyle,			// Window Style
			0, 0,				// Window X,Y Position
			winRect.right - winRect.left,	// Window Width
			winRect.bottom - winRect.top,	// Window Height
			HWND_DESKTOP,	// Desktop Is Window's Parent
			0,			// No Menu
			instance,  // Pass The Window Instance
			NULL); // pointer to window class
		app_->SetWindow(hwnd);
#endif

		if (app_->InitApi())
		{
			if (app_->Load())
			{
				// Show window
#if defined(TARGET_WINDOWS)
				ShowWindow(hwnd, SW_NORMAL);
#endif
				app_->set_visible(true);

				//	Some dual core systems have a problem where the different CPUs return different
				//	QueryPerformanceCounter values. So when this thread is schedule on the other CPU
				//	in a later frame, we could even get a negative frameTime. To solve this we force
				//	the main thread to always run on CPU 0.
#if defined(TARGET_WINDOWS)
				SetThreadAffinityMask(GetCurrentThread(), 1);
#endif

				// Start time
				update_timer.Start();

				// program main loop
#if defined(TARGET_WINDOWS)
				MSG	msg;
#endif
				bool bQuit = false;
				while (!bQuit)
				{
					// calculate dt
					app_->SetFrameTime(update_timer.GetElapsedTime());

#if defined(TARGET_WINDOWS)
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
#endif
					if (bQuit) break;

					app_->Update();

					app_->BeginFrame();
					app_->Render();
					app_->EndFrame();
				}
			}
			else // failed to load
			{
				app_->Unload(); // delete allocated objects
				app_->DeinitApi();
			}
		}
		else
		{
#if defined(TARGET_WINDOWS)
			MessageBox(HWND_DESKTOP, TEXT("Failed to init API!"), TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
			DestroyWindow(hwnd);
#endif
		}

		if (app_->fullscreen())
		{
#if defined(TARGET_WINDOWS)
			ChangeDisplaySettings(NULL, 0);
#endif
		}

#if defined(TARGET_WINDOWS)
		UnregisterClassA(app_class_name, instance);
		DestroyIcon(app_->icon_);
#endif
	}
	void Application::Terminate()
	{
#if defined(TARGET_WINDOWS)
		PostMessage((HWND)hwnd_, WM_CLOSE, 0, 0);
#elif defined(TARGET_MAC)
		//static_assert(false, "Application::Terminate has not been defined");
#elif defined(TARGET_UNIX)
		static_assert(false, "Application::Terminate has not been defined");
#endif
	}
	const char* Application::GetIniFilePath()
	{
		return "config.ini";
	}
	void Application::ToggleFullscreen(void)
	{
		if (fullscreen_) // fullscreen -> windowed
		{
			MakeWindowed();
			fullscreen_ = false;
		}
		else // windowed -> fullscreen
		{
			if (MakeFullscreen())
				fullscreen_ = true;
			else
				fullscreen_ = false;
		}
	}
	bool Application::MakeFullscreen(void)
	{
#ifdef TARGET_WINDOWS
		if (!fullscreen_) return false;

		DEVMODE dmScreenSettings;
		ZeroMemory(&dmScreenSettings, sizeof(DEVMODE));
		dmScreenSettings.dmSize = sizeof(DEVMODE);
		dmScreenSettings.dmPelsWidth = width_;
		dmScreenSettings.dmPelsHeight = height_;
		dmScreenSettings.dmBitsPerPel = color_bits_;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			// Still windowed
			fullscreen_ = false;
			SetWindowLongPtr(hwnd_, GWL_STYLE, WS_OVERLAPPEDWINDOW);
			SetWindowLongPtr(hwnd_, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_DLGMODALFRAME);
			ShowWindow(hwnd_, SW_NORMAL);
			return false;
		}

		SetWindowLongPtr(hwnd_, GWL_STYLE, WS_POPUP);
		SetWindowLongPtr(hwnd_, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_TOPMOST);
		ShowWindow(hwnd_, SW_NORMAL);
#endif
		return true;
	}
	void Application::MakeWindowed(void)
	{
		if (!fullscreen_) return;

#ifdef TARGET_WINDOWS
		ChangeDisplaySettings(NULL, 0); // restore display settings

		SetWindowLongPtr(hwnd_, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		SetWindowLongPtr(hwnd_, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_DLGMODALFRAME);
		ShowWindow(hwnd_, SW_NORMAL);
#endif
	}
	void Application::InitWindowSize(int w, int h, bool fullscr)
	{
		width_ = w;
		height_ = h;
		aspect_ratio_ = (float)width_ / (float)height_;
		fullscreen_ = fullscr;
		ComputeFramebufferSize();
	}
	void Application::SetWindowTitle(const char* title)
	{
#ifdef TARGET_WINDOWS
		SetWindowTextA(hwnd_, title);
#endif
	}
	void Application::SetFrameTime(float ftime)
	{
		// Increase time
		time_ += ftime;
		// Set value
		frame_time_ = ftime;

#ifdef _DEBUG
		// Clamp update value when debugging step by step
		if (frame_time_ > 1.0f)
			frame_time_ = 0.0166f;
#endif

		// Compute current frame rate
		if (fps_counter_time_ < 1.0f)
		{
			fps_counter_count_ += 1.0f;
			fps_counter_time_ += frame_time_;
		}
		else
		{
			frame_rate_ = fps_counter_count_ / fps_counter_time_;
			fps_counter_count_ = 0.0f;
			fps_counter_time_ = 0.0f;
		}
	}
	void Application::GetMousePos(int& x, int& y)
	{
#ifdef TARGET_WINDOWS
		RECT winrect;
		GetWindowRect(hwnd_, &winrect);
		POINT p;
		GetCursorPos(&p);
		x = p.x - winrect.left; // [0, w]
		y = winrect.bottom - p.y; // [0, h]
#endif
	}
	void Application::SetMousePos(int x, int y)
	{
#ifdef TARGET_WINDOWS
		RECT winrect;
		GetWindowRect(hwnd_, &winrect);
		SetCursorPos((int)winrect.left + x, (int)winrect.bottom - y);
#endif
	}
	void Application::MouseToCenter(int &px, int &py)
	{
#ifdef TARGET_WINDOWS
		RECT winrect;
		GetWindowRect(hwnd_, &winrect);
		px = width_ / 2;
		py = height_ / 2;
		SetCursorPos((int)winrect.left + px, (int)winrect.bottom - py);
#endif
	}
	void Application::ComputeFramebufferSize()
	{
		int size = std::max(width_, height_);
		if (size <= 1024)
			framebuffer_size_ = 1024;
		else if (size <= 2048)
			framebuffer_size_ = 2048;
		else
			framebuffer_size_ = 4096;
		inv_framebuffer_size_ = 1.0f / (float)framebuffer_size_;
	}
	bool Application::PreStartInit(void)
	{
		return true;
	}
	bool Application::Load()
	{
		return true;
	}
	void Application::Unload()
	{
	}
	void Application::Update()
	{
		//if (keys_.key_down[VK_ESCAPE])
		//	Application::Terminate();
	}
	void Application::Render()
	{
	}
	const char* Application::GetTitle(void)
	{
		return "Test application";
	}
	const bool Application::IsClampFps(void)
	{
		return false;
	}
	void Application::OnLButtonDown(void)
	{
	}
	void Application::OnLButtonUp(void)
	{
	}
	void Application::OnMButtonDown(void)
	{
	}
	void Application::OnMButtonUp(void)
	{
	}
	void Application::OnRButtonDown(void)
	{
	}
	void Application::OnRButtonUp(void)
	{
	}
	void Application::OnMouseMove(void)
	{
	}
	void Application::OnMouseWheel(int delta)
	{
	}
	void Application::OnSize(int w, int h)
	{
		width_ = w;
		height_ = h;
		aspect_ratio_ = (float)width_ / (float)height_;
		renderer_->UpdateSizes(width_, height_);
	}
	bool Application::visible()
	{
		return visible_;
	}
	bool Application::fullscreen()
	{
		return fullscreen_;
	}
	Keys& Application::keys()
	{
		return keys_;
	}
	void Application::set_visible(bool vis)
	{
		visible_ = vis;
	}
	int Application::width()
	{
		return width_;
	}
	int Application::height()
	{
		return height_;
	}
	int Application::color_bits()
	{
		return color_bits_;
	}

} // namespace sht