#pragma once
#ifndef __SHT_GRAPHICS_RENDERER_TEXTURE_H__
#define __SHT_GRAPHICS_RENDERER_TEXTURE_H__

#include "../../../common/types.h"
#include "../resource.h"
#include "../../../graphics/include/image/image.h"

namespace sht {
	namespace graphics {

		//! Texture class
		class Texture : public Resource {
            friend class Font;
			friend class Renderer;
			friend class OpenGlRenderer;

		public:

			//! Texture filtering types
			enum class Filter {
				kPoint,
				kLinear,
				kBilinear,
				kTrilinear,
				kBilinearAniso,
				kTrilinearAniso
			};

			//! Texture wrapping types
			enum class Wrap {
				kClamp,
				kClampToEdge,
				kRepeat
			};

			virtual u32 GetSrcFormat() = 0;
			virtual u32 GetSrcType() = 0;
			virtual s32 GetInternalFormat() = 0;

			u32 GetSize();			//!< size of image in memory (w*h*bpp)

		protected:
			Texture();
			virtual ~Texture();
			Texture(const Texture&) = delete;
			void operator = (const Texture&) = delete;

			virtual void ChooseTarget() = 0;

			u32 target_;			//!< texture target
			u32 texture_id_;		//!< texture ID
			u32 depth_id_;			//!< depth id
			u32 stencil_id_;		//!< stencil id
			Image::Format format_;	//!< image format type
			int width_;				//!< width of texture
			int height_;			//!< height of texture
		};

	} // namespace graphics
} // namespace sht

#endif