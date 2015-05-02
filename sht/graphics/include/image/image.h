#pragma once
#ifndef __SHT_GRAPHICS_IMAGE_IMAGE_H__
#define __SHT_GRAPHICS_IMAGE_IMAGE_H__

#include "../../../common/types.h"
#include "../../../common/platform.h"

namespace sht {
	namespace graphics {

		//! Image class
		class Image {
		public:
			enum class Format {
				kNone,
				kI8, kI16, kI32,
				kA8, kA16, kA32,
				kL8, kL16, kL32,
				kLA8, kLA16, kLA32,
				kR8, kR16, kR32,
				kRG8, kRG16, kRG32,
				kRGB8, kRGB16, kRGB32,
				kRGBA8, kRGBA16, kRGBA32,
				kDepth16, kDepth24, kDepth32
			};

			enum class DataType {
				kBmp, kJpg, kPng, kTga, kTif
			};

			Image();
			~Image();

			u8* pixels();
			Format format();
			int width();
			int height();
			int bpp();

			u8* Allocate(int w, int h, Format fmt);						//!< allocates a place for image data and returns its data pointer
			void Save(DataType fmt, const char* filename);				//!< saves image to file with specified format

			bool LoadFromFile(const char* filename);					//!< loads image from file
			bool LoadCubemapFromFile(const char* filename, int ind);	//!< loads cubemap part with specified index from file
			bool LoadNMapFromHMap(const char* filename);				//!< loads normalmap from heightmap file
			bool LoadNHMapFromHMap(const char* filename);				//!< loads normalheightmap from heightmap file

		private:

			u8 *pixels_;		//!< bytes of the source image
			Format format_;		//!< pixel format of the source image
			int width_;			//!< width of the source image
			int height_;		//!< height of the source image
			int bpp_;			//!< number of BYTES per pixel
		};

	} // namespace graphics
} // namespace sht

#endif