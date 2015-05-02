#pragma once
#ifndef __SHT_GRAPHICS_RENDERER_OPENGL_TEXTURE_H__
#define __SHT_GRAPHICS_RENDERER_OPENGL_TEXTURE_H__

#include "../texture.h"

namespace sht {
	namespace graphics {

		//! Texture class for OpenGL
		class OpenGlTexture : public Texture {
		public:
			u32 GetSrcFormat();
			u32 GetSrcType();
			s32 GetInternalFormat();

		private:
			void ChooseTarget();
		};

	} // namespace graphics
} // namespace sht

#endif