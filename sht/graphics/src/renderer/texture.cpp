#include "../../include/renderer/texture.h"

namespace sht {
	namespace graphics {

		Texture::Texture()
		{

		}
		Texture::~Texture()
		{

		}
		u32 Texture::GetSize()
		{
			u32 s;
			switch (format_)
			{
			case Image::Format::kR8: s = 1;
			case Image::Format::kR16: s = 2;
			case Image::Format::kR32: s = 4;
			case Image::Format::kRG8: s = 2;
			case Image::Format::kRG16: s = 4;
			case Image::Format::kRG32: s = 8;
			case Image::Format::kRGBA8: s = 4;
			case Image::Format::kRGBA16: s = 8;
			case Image::Format::kRGBA32: s = 16;
			case Image::Format::kRGB8: s = 3;
			case Image::Format::kRGB16: s = 6;
			case Image::Format::kRGB32: s = 12;
			case Image::Format::kA8:
			case Image::Format::kI8:
			case Image::Format::kL8:
				s = 1;
			case Image::Format::kA16:
			case Image::Format::kI16:
			case Image::Format::kL16:
				s = 2;
			case Image::Format::kA32:
			case Image::Format::kI32:
			case Image::Format::kL32:
				s = 4;
			case Image::Format::kLA8: s = 2;
			case Image::Format::kLA16: s = 4;
			case Image::Format::kLA32: s = 8;
			case Image::Format::kDepth16: s = 2;
			case Image::Format::kDepth24: s = 3;
			case Image::Format::kDepth32: s = 4;
			default: s = 4;
			}
			s *= (u32)(width_ * height_);
			return s;
		}

	} // namespace graphics
} // namespace sht