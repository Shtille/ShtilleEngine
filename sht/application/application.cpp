#include "application.h"
#include "../platform/include/main_wrapper.h"
#include "../platform/include/window_controller.h"
#include "../graphics/include/renderer/renderer.h"
#include "../system/include/memory_leaks.h"
#include "../system/include/stream/file_stream.h"

#include "../system/include/time/time_manager.h"
#include "../utility/include/resource_manager.h"

#include <cstdlib>
#include <clocale>
#include <algorithm> // for std::max

namespace sht {

	Application * Application::app_ = nullptr;

	Application::Application()
	: renderer_(nullptr)
	, visible_(false)
	, fullscreen_(false)
	, width_(800)
	, height_(600)
	, aspect_ratio_(1.0f)
	, color_bits_(32)
	, depth_bits_(24)
	, stencil_bits_(0)
	, need_take_screenshot_(false)
	, frame_time_(0.0f)
	, frame_rate_(0.0f)
	, framebuffer_size_(0)
	, inv_framebuffer_size_(0.0f)
	{

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

		// Set fixed frame time via desired frame rate
		frame_time_ = 1.0f / GetDesiredFrameRate();
		frame_rate_ = GetDesiredFrameRate();
		
		// Set proper text encoding to let use non-english characters
		setlocale(LC_CTYPE, "UTF-8");

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
        
		return PlatformWindowMakeFullscreen();
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
	void Application::InitializeManagers()
	{
		sht::system::TimeManager::CreateInstance();
		sht::utility::ResourceManager::CreateInstance();

		// Our engine uses fixed time steps, so make it shared for any consumer
		sht::system::TimeManager::GetInstance()->SetFixedFrameTime(frame_time_);
	}
	void Application::DeinitializeManagers()
	{
		sht::utility::ResourceManager::DestroyInstance();
		sht::system::TimeManager::DestroyInstance();
	}
	void Application::UpdateManagers()
	{
		// Update time manager
		sht::system::TimeManager::GetInstance()->Update();
	}
	bool Application::PreStartInit()
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
	void Application::UpdatePhysics(float sec)
	{
	}
	void Application::Render()
	{
	}
	const char* Application::GetTitle()
	{
		return "Test application";
	}
    const bool Application::IsMultisample()
    {
        return false;
    }
	const bool Application::IsBenchmark()
	{
		return false;
	}
	const bool Application::IsResizable()
	{
		return false;
	}
	const bool Application::IsDecorated()
	{
		return true;
	}
	const float Application::GetDesiredFrameRate()
	{
		return 60.0f;
	}
    void Application::OnChar(unsigned short code)
    {
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
            renderer_->SetViewport(width_, height_);
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
	u32 Application::color_bits()
	{
		return static_cast<u32>(color_bits_);
	}
    u32 Application::depth_bits()
    {
        return static_cast<u32>(depth_bits_);
    }
    u32 Application::stencil_bits()
    {
        return static_cast<u32>(stencil_bits_);
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