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

			enum class FileFormat {
				kUnknown,
				kBmp, kJpg, kPng, kTga, kTif, kHdr
			};

			enum class DataType {
				kUint8, kUint16, kUint32,
				kHalfFloat, kFloat, kDouble
			};

			Image();
			~Image();

			u8* pixels();
			Format format();
			int width();
			int height();
			int bpp();

			u8* Allocate(int w, int h, Format fmt);						//!< allocates a place for image data and returns its data pointer
            void FillWithZeroes();
            void SubData(int offset_x, int offset_y, int w, int h, const u8* data);
			bool Save(const char* filename);							//!< saves image to file with specified format

			bool LoadFromFile(const char* filename);					//!< loads image from file
			bool LoadCubemapFromFile(const char* filename, int ind);	//!< loads cubemap part with specified index from file
			bool LoadNMapFromHMap(const char* filename);				//!< loads normalmap from heightmap file
			bool LoadNHMapFromHMap(const char* filename);				//!< loads normalheightmap from heightmap file
			
			void Rescale(int w, int h);									//!< rescales stored image
			void MakePowerOfTwo();										//!< rescales image to be power of two in each size

		protected:
			void SwapRedBlueChannels();

			// Save routines
			bool SaveJpeg(const char *filename, int quality = 75);
			bool SavePng(const char *filename);
			bool SaveBmp(const char *filename);
			bool SaveTiff(const char *filename);
			bool SaveTga(const char *filename);
			bool SaveHdr(const char *filename);

			// Load routines
			bool LoadJpeg(const char *filename);
			bool LoadPng(const char *filename);
			bool LoadBmp(const char *filename);
			bool LoadTiff(const char *filename);
			bool LoadTga(const char *filename);
			bool LoadHdr(const char *filename);

		private:

			u8 *pixels_;		//!< bytes of the source image
			Format format_;		//!< pixel format of the source image
			DataType data_type_;//!< type of pixel data of the source image
			int width_;			//!< width of the source image
			int height_;		//!< height of the source image
			int channels_;		//!< number of channels in image
			int bpp_;			//!< number of BYTES per pixel
		};

	} // namespace graphics
} // namespace sht

#endif