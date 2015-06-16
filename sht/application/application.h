#pragma once
#ifndef __SHT_APPLICATION_APPLICATION_H__
#define __SHT_APPLICATION_APPLICATION_H__

#include "../common/platform.h"

namespace sht {

	// Predeclarations
	namespace graphics {
		class Renderer;
	}

	struct Keys {
		bool key_down[256];
		bool key_active[256];
		char key_queue[64];
		int key_queue_size;
	};

	//! Base class for application
	class Application {
	public:
		Application();
		virtual ~Application();

#ifdef TARGET_WINDOWS
		void Run();
#else
        void Run(int argc, const char** argv);
#endif
		void Terminate();

		const char* GetIniFilePath();

		// Fullscreen operations
		void ToggleFullscreen(void);
		bool MakeFullscreen(void);
		void MakeWindowed(void);

		void InitWindowSize(int w, int h, bool fullscr);
#ifdef TARGET_WINDOWS
		void SetWindow(HWND wnd) { hwnd_ = wnd; }
#endif
		void SetWindowTitle(const char* title);
		void SetFrameTime(float ftime);

		void GetMousePos(int& x, int& y); // x=[0,w]; y=[0,h]
		void SetMousePos(int x, int y); // x=[0,a]; y=[0,1]
		void MouseToCenter(int &px, int &py);

		// API specific functions
		virtual bool ShowStartupOptions() = 0;
		virtual bool InitApi() = 0;
		virtual void DeinitApi() = 0;
		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		bool visible();
		bool fullscreen();
		Keys& keys();
		int width();
		int height();
		int color_bits();

		void set_visible(bool vis);

		// --- User defined functions ---

		// User data loading/unloading
		virtual bool PreStartInit(void); //!< All window initialization goes here
		virtual bool Load();
		virtual void Unload();

		virtual void Update();
		virtual void Render();

		// Application parameters
		virtual const char* GetTitle(void);
		virtual const bool IsClampFps(void);

		// Messages
		virtual void OnLButtonDown(void);
		virtual void OnLButtonUp(void);
		virtual void OnMButtonDown(void);
		virtual void OnMButtonUp(void);
		virtual void OnRButtonDown(void);
		virtual void OnRButtonUp(void);
		virtual void OnMouseMove(void);
		virtual void OnMouseWheel(int delta);
		virtual void OnSize(int w, int h);

	protected:

		void ComputeFramebufferSize();

#ifdef TARGET_WINDOWS
		HWND hwnd_;						//!< window handle
		HICON icon_;					//!< window icon
#endif
		class sht::graphics::Renderer *renderer_;
		Keys keys_;						//!< keys information
		bool visible_;					//!< is window visible
		bool fullscreen_;				//!< is window fullscreen
		int width_;						//!< width of the window
		int height_;					//!< height of the window
		float aspect_ratio_;			//!< aspect ratio of window
		unsigned char color_bits_;				//!< number of bits for color buffer
		unsigned char depth_bits_;				//!< number of bits for depth buffer
		float time_;					//!< total time elapsed from application start
		float frame_time_;				//!< time elapsed since last update
		float frame_rate_;				//!< frames per second (FPS)
		float fps_counter_time_;		//!< for counting FPS
		float fps_counter_count_;		//!< for counting FPS
		bool need_take_screenshot_;		//!< do we need to take a screenshot?
		int framebuffer_size_;			//!< optimal size for framebuffer
		float inv_framebuffer_size_;	//!< inverted framebuffer size for shaders

	private:
#ifdef TARGET_WINDOWS
		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		static bool RegisterWindowClass(HINSTANCE inst, const char* appclassname);
		static void WindowsErrorCheck();
#endif
		static Application *app_;
	};

} // namespace sht

#endif