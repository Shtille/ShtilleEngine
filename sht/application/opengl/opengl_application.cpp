#include "opengl_application.h"
#include "../../graphics/include/renderer/opengl/opengl_renderer.h"
#include "../../platform/include/window_controller.h"

namespace sht {

	OpenGlApplication::OpenGlApplication()
	{

	}
	OpenGlApplication::~OpenGlApplication()
	{

	}
	bool OpenGlApplication::ShowStartupOptions()
	{
		return true;
	}
	bool OpenGlApplication::InitApi()
	{
		if (!PlatformInitOpenGLContext(color_bits_, depth_bits_, stencil_bits_))
			return false;

		PlatformMakeContextCurrent();

		if (IsBenchmark())
			PlatformSwapInterval(0);

		sht::graphics::OpenGlRenderer * renderer = new sht::graphics::OpenGlRenderer(width_, height_);
		renderer_ = renderer;

		return true;
	}
	void OpenGlApplication::DeinitApi()
	{
		renderer_->CleanUp();
		delete renderer_;

		PlatformDeinitOpenGLContext();
	}
	void OpenGlApplication::BeginFrame()
	{
		renderer_->Defaults();
	}
	void OpenGlApplication::EndFrame()
	{
		PlatformSwapBuffers();
	}

} // namespace sht