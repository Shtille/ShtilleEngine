#include "../../../include/renderer/opengl/opengl_texture.h"
#include "opengl_include.h"

namespace sht {
	namespace graphics {

		u32 OpenGlTexture::GetSrcFormat()
		{
			switch (format_)
			{
			case Image::Format::kRGBA8:
			case Image::Format::kRGBA16:
			case Image::Format::kRGBA32:
				return GL_RGBA;
			case Image::Format::kRGB8:
			case Image::Format::kRGB16:
			case Image::Format::kRGB32:
				return GL_RGB;
			case Image::Format::kR8:
			case Image::Format::kR16:
			case Image::Format::kR32:
				return GL_RED;
			case Image::Format::kRG8:
			case Image::Format::kRG16:
			case Image::Format::kRG32:
				return GL_RG;
			case Image::Format::kA8:
			case Image::Format::kA16:
			case Image::Format::kA32:
				return GL_ALPHA;
			case Image::Format::kI8:
			case Image::Format::kI16:
			case Image::Format::kI32:
				return GL_INTENSITY;
			case Image::Format::kL8:
			case Image::Format::kL16:
			case Image::Format::kL32:
				return GL_LUMINANCE;
			case Image::Format::kLA8:
			case Image::Format::kLA16:
			case Image::Format::kLA32:
				return GL_LUMINANCE_ALPHA;
			case Image::Format::kDepth16:
			case Image::Format::kDepth24:
			case Image::Format::kDepth32:
				return GL_DEPTH_COMPONENT;
			default:
				return GL_RGB;
			}
		}
		u32 OpenGlTexture::GetSrcType()
		{
			switch (format_)
			{
			case Image::Format::kR8:
			case Image::Format::kRG8:
			case Image::Format::kRGB8:
			case Image::Format::kRGBA8:
			case Image::Format::kA8:
			case Image::Format::kI8:
			case Image::Format::kL8:
			case Image::Format::kLA8:
				return GL_UNSIGNED_BYTE;
			case Image::Format::kR16:
			case Image::Format::kRG16:
			case Image::Format::kRGB16:
			case Image::Format::kRGBA16:
			case Image::Format::kA16:
			case Image::Format::kI16:
			case Image::Format::kL16:
			case Image::Format::kLA16:
				return GL_HALF_FLOAT_ARB;
			case Image::Format::kR32:
			case Image::Format::kRG32:
			case Image::Format::kRGB32:
			case Image::Format::kRGBA32:
			case Image::Format::kA32:
			case Image::Format::kI32:
			case Image::Format::kL32:
			case Image::Format::kLA32:
			case Image::Format::kDepth16:
			case Image::Format::kDepth24:
			case Image::Format::kDepth32:
				return GL_FLOAT;
			default:
				return GL_UNSIGNED_BYTE;
			}
		}
		s32 OpenGlTexture::GetInternalFormat()
		{
			switch (format_)
			{
			case Image::Format::kRGBA8: return GL_RGBA8;
			case Image::Format::kRGBA16: return GL_RGBA16F_ARB;
			case Image::Format::kRGBA32: return GL_RGBA32F_ARB;
			case Image::Format::kRGB8: return GL_RGB8;
			case Image::Format::kRGB16: return GL_RGB16F_ARB;
			case Image::Format::kRGB32: return GL_RGB32F_ARB;
			case Image::Format::kR8: return GL_R8;
			case Image::Format::kR16: return GL_R16F;
			case Image::Format::kR32: return GL_R32F;
			case Image::Format::kRG8: return GL_RG8;
			case Image::Format::kRG16: return GL_RG16F;
			case Image::Format::kRG32: return GL_RG32F;
			case Image::Format::kA8: return GL_ALPHA;
			case Image::Format::kA16: return GL_ALPHA16F_ARB;
			case Image::Format::kA32: return GL_ALPHA32F_ARB;
			case Image::Format::kI8: return GL_INTENSITY8I_EXT;
			case Image::Format::kI16: return GL_INTENSITY16F_ARB;
			case Image::Format::kI32: return GL_INTENSITY32F_ARB;
			case Image::Format::kL8: return GL_LUMINANCE8I_EXT;
			case Image::Format::kL16: return GL_LUMINANCE16F_ARB;
			case Image::Format::kL32: return GL_LUMINANCE32F_ARB;
			case Image::Format::kLA8: return GL_LUMINANCE_ALPHA8I_EXT;
			case Image::Format::kLA16: return GL_LUMINANCE_ALPHA16F_ARB;
			case Image::Format::kLA32: return GL_LUMINANCE_ALPHA32F_ARB;
			case Image::Format::kDepth16: return GL_DEPTH_COMPONENT16_ARB;
			case Image::Format::kDepth24: return GL_DEPTH_COMPONENT24_ARB;
			case Image::Format::kDepth32: return GL_DEPTH_COMPONENT32_ARB;
			default: return GL_RGBA8;
			}
		}
		void OpenGlTexture::ChooseTarget()
		{
			target_ = GL_TEXTURE_2D;
		}

	} // namespace graphics
} // namespace sht