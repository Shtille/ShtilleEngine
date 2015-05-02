#pragma once
#ifndef __SHT_APPLICATION_OPENGL_APPLICATION_H__
#define __SHT_APPLICATION_OPENGL_APPLICATION_H__

#include "../application.h"

namespace sht {

	class OpenGlApplication : public Application {
	public:
		OpenGlApplication();
		virtual ~OpenGlApplication();

		bool ShowStartupOptions() final;
		bool InitApi() final;
		void DeinitApi() final;
		void BeginFrame() final;
		void EndFrame() final;

	protected:
#ifdef TARGET_WINDOWS
		HDC dc_;	//!< device context
		HGLRC rc_;	//!< rendering context
#endif
	};

} // namespace sht

#endif