#include "opengl_application.h"
#include "../../graphics/include/renderer/opengl/opengl_renderer.h"
#include "../../platform/include/window_wrapper.h"

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
		if (!PlatformInitOpenGLContext(color_bits_, depth_bits_))
			return false;

		sht::graphics::OpenGlRenderer * renderer = new sht::graphics::OpenGlRenderer(width_, height_);
		renderer_ = renderer;

		return renderer->CheckFunctionalities();
	}
	void OpenGlApplication::DeinitApi()
	{
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