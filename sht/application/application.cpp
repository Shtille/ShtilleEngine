#include "application.h"
#include <memory.h>
#include "../graphics/include/renderer/renderer.h"
#include "../system/include/memory_leaks.h"
#include "../system/include/stream/file_stream.h"
#include <cstdlib>
#if defined(TARGET_MAC)
#include <AGL/AGL.h>
#endif
#include <algorithm> // for std::max
#ifdef TARGET_WINDOWS
#undef max
#endif
//#if defined(TARGET_MAC) || defined(TARGET_IOS)
#include "../platform/include/main_wrapper.h"
#include "../platform/include/window_wrapper.h"
//#endif

namespace sht {

	Application * Application::app_ = nullptr;

	Application::Application()
	{
		visible_ = false;
		fullscreen_ = false;
		color_bits_ = 32;
		depth_bits_ = 32;

		time_ = 0.0f;
		frame_time_ = 0.0f;

		fps_counter_time_ = 0.0f;
		fps_counter_count_ = 0.0f;
		need_take_screenshot_ = false;
	}
	Application::~Application()
	{

	}
    Application* Application::GetInstance()
    {
        return app_;
    }
    int Application::Run(int argc, const char** argv)
	{
		app_ = this;

		// Enable automatic memory leaks checking
        sht::system::EnableMemoryLeaksChecking();

		// Prestart initialization
		if (!ShowStartupOptions()) // TODO: seems like obsolete, delete this
		{
			return 1;
		}
		if (!PreStartInit())
		{
			return 2;
		}

        // Read window settings from file
		sht::system::FileStream ini_file;
		if (ini_file.Open(GetIniFilePath(), sht::system::StreamAccess::kReadText))
		{
			char temp[10]; int w, h; bool fullscr;

			ini_file.ScanString("%i x %i", &w, &h);
			ini_file.ScanString("%s", temp, _countof(temp));
			ini_file.Close();

			fullscr = strcmp(temp, "TRUE") == 0;

			InitWindowSize(w, h, fullscr);
		}
		else
		{
			InitWindowSize(800, 600, false);
		}
        
        // This code is for Mac OS X and iOS.
        // Under these platforms all initializing code doing in coressponding classes.
        PlatformChangeDirectoryToResources();

        // Wrapper for the Mac OS X and iOS main function
        return MainWrapper(argc, argv);
	}
	void Application::Terminate()
	{
        PlatformWindowTerminate();
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
        if (fullscreen_) return true;
        PlatformWindowMakeFullscreen();
        //fullscreen_ = false;
		return true;
	}
	void Application::MakeWindowed(void)
	{
		if (!fullscreen_) return;

        PlatformWindowMakeWindowed();
	}
	void Application::Center()
	{
		PlatformWindowCenter();
	}
    void Application::Resize(int width, int height)
    {
        PlatformWindowResize(width, height);
    }
    void Application::SetTitle(const char* title)
    {
        PlatformWindowSetTitle(title);
    }
    void Application::Iconify()
    {
        PlatformWindowIconify();
    }
    void Application::Restore()
    {
        PlatformWindowRestore();
    }
    void Application::Show()
    {
        PlatformWindowShow();
    }
    void Application::Hide()
    {
        PlatformWindowHide();
    }
	void Application::InitWindowSize(int w, int h, bool fullscr)
	{
		width_ = w;
		height_ = h;
		aspect_ratio_ = (float)width_ / (float)height_;
		fullscreen_ = fullscr;
		ComputeFramebufferSize();
	}
	void Application::SetFrameTime(float ftime)
	{
		// Increase time
		time_ += ftime;
		// Set value
		frame_time_ = ftime;

#if defined(_DEBUG) || defined(DEBUG)
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
	void Application::GetCursorPos(float& x, float& y)
	{
        PlatformGetCursorPos(x, y);
	}
	void Application::SetCursorPos(float x, float y)
	{
        PlatformSetCursorPos(x, y);
	}
	void Application::CursorToCenter()
	{
        PlatformMouseToCenter();
	}
    void Application::ShowCursor()
    {
        PlatformShowCursor();
    }
    void Application::HideCursor()
    {
        PlatformHideCursor();
    }
    void Application::SetClipboardText(const char *text)
    {
        PlatformSetClipboardText(text);
    }
    std::string Application::GetClipboardText()
    {
        return PlatformGetClipboardText();
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
    void Application::OnKeyDown(sht::PublicKey key, int modifiers)
    {
    }
    void Application::OnKeyUp(sht::PublicKey key, int modifiers)
    {
    }
	void Application::OnMouseDown(sht::MouseButton button, int modifiers)
	{
	}
	void Application::OnMouseUp(sht::MouseButton button, int modifiers)
	{
	}
	void Application::OnMouseMove(void)
	{
	}
	void Application::OnScroll(float delta_x, float delta_y)
	{
	}
	void Application::OnSize(int w, int h)
	{
		width_ = w;
		height_ = h;
		aspect_ratio_ = (float)width_ / (float)height_;
        if (renderer_) // renderer may not be initialized yet
        {
            renderer_->UpdateSizes(width_, height_);
            renderer_->Viewport(width_, height_);
        }
        // TODO: what to do if framebuffer size changes during executing?
        ComputeFramebufferSize();
	}
	bool Application::visible()
	{
		return visible_;
	}
	bool Application::fullscreen()
	{
		return fullscreen_;
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
    Keys& Application::keys()
    {
        return keys_;
    }
    Mouse& Application::mouse()
    {
        return mouse_;
    }

} // namespace sht