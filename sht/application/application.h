#pragma once
#ifndef __SHT_APPLICATION_APPLICATION_H__
#define __SHT_APPLICATION_APPLICATION_H__

#include "../common/platform.h"
#include "../graphics/include/renderer/renderer.h"
#include "../system/include/keys.h"
#include "../system/include/mouse.h"

namespace sht {

	// Predeclarations
	namespace graphics {
		class Renderer;
	}

	//! Base class for application
	class Application {
	public:
		Application();
		virtual ~Application();
        
        static Application* GetInstance(); //!< it's not a singleton function
        
        const char* GetIniFilePath();

        int Run(int argc, const char** argv);
		void Terminate();

		// Window operations
		void ToggleFullscreen(void);
		bool MakeFullscreen(void);
		void MakeWindowed(void);
		void Center();
        void Resize(int width, int height);
        void SetTitle(const char* title);
        void Iconify();
        void Restore();
        void Show();
        void Hide();
        
        // Cursor operations
        void GetCursorPos(float& x, float& y); // x=[0,w]; y=[0,h]
        void SetCursorPos(float x, float y); // x=[0,a]; y=[0,1]
        void CursorToCenter();
        void ShowCursor();
        void HideCursor();
        
        // Clipboard operations
        void SetClipboardText(const char *text);
        std::string GetClipboardText();

		void SetFrameTime(float ftime);

		// API specific functions
		virtual bool ShowStartupOptions() = 0;
		virtual bool InitApi() = 0;
		virtual void DeinitApi() = 0;
		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		bool visible();
		bool fullscreen();
		int width();
		int height();
		int color_bits();
        
        Keys& keys();
        Mouse& mouse();

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
		virtual const bool IsClampFps(void); //!< obsolete?

		// --- Messages ---
        virtual void OnKeyDown(sht::PublicKey key, int modifiers);
        virtual void OnKeyUp(sht::PublicKey key, int modifiers);
        virtual void OnMouseDown(sht::MouseButton button, int modifiers);
		virtual void OnMouseUp(sht::MouseButton button, int modifiers);
		virtual void OnMouseMove(void);
		virtual void OnScroll(float delta_x, float delta_y);
		virtual void OnSize(int w, int h);

	protected:

		void ComputeFramebufferSize();

		sht::graphics::Renderer *renderer_; //!< our renderer object
        Keys keys_;                     //!< keys information
        Mouse mouse_;                   //!< mouse information
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
        
        void InitWindowSize(int w, int h, bool fullscr);

		static Application *app_;
	};

} // namespace sht

#endif