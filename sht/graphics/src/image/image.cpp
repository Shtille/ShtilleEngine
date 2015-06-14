#include "../../include/image/image.h"
#include "../../../system/include/string/filename.h"
#include <assert.h>
#include <math.h>
#include <memory.h>

namespace sht {
	namespace graphics {

		static int GetBpp(Image::Format fmt)
			// Number of bits per pixel
		{
			switch (fmt)
			{
			case Image::Format::kA8:
			case Image::Format::kI8:
			case Image::Format::kL8:
			case Image::Format::kR8:
				return 8;
			case Image::Format::kA16:
			case Image::Format::kI16:
			case Image::Format::kL16:
			case Image::Format::kR16:
				return 16;
			case Image::Format::kA32:
			case Image::Format::kI32:
			case Image::Format::kL32:
			case Image::Format::kR32:
				return 32;
			case Image::Format::kLA8:
			case Image::Format::kRG8:
				return 16;
			case Image::Format::kLA16:
			case Image::Format::kRG16:
				return 32;
			case Image::Format::kLA32:
			case Image::Format::kRG32:
				return 64;
			case Image::Format::kRGB8:
				return 24;
			case Image::Format::kRGB16:
				return 48;
			case Image::Format::kRGB32:
				return 96;
			case Image::Format::kRGBA8:
				return 32;
			case Image::Format::kRGBA16:
				return 64;
			case Image::Format::kRGBA32:
				return 128;
			default:
				assert(false && "unknown image format");
				return 24;
			}
		}
		static Image::FileFormat ExtractFileFormat(const char* filename)
		{
			sht::system::Filename fn(filename);
			std::string ext = fn.ExtractExt();
			if (ext == "bmp")
				return Image::FileFormat::kBmp;
			else if (ext == "jpg")
				return Image::FileFormat::kJpg;
			else if (ext == "png")
				return Image::FileFormat::kPng;
			else if (ext == "tga")
				return Image::FileFormat::kTga;
			else if (ext == "tif" || ext == "tiff")
				return Image::FileFormat::kTif;
			return Image::FileFormat::kUnknown;
		}
		Image::Image() : pixels_(nullptr)
		{
		}
		Image::~Image()
		{
			if (pixels_) delete[] pixels_;
		}
		u8* Image::pixels()
		{
			return pixels_;
		}
		Image::Format Image::format()
		{
			return format_;
		}
		int Image::width()
		{
			return width_;
		}
		int Image::height()
		{
			return height_;
		}
		int Image::bpp()
		{
			return bpp_;
		}
		void Image::SwapRedBlueChannels()
		{
			int unit_size = bpp_ / channels_;
			u8 *temp = new u8[unit_size];
			for (int i = 0; i < width_*height_*bpp_; i += bpp_)
			{
				memcpy(temp, &pixels_[i], unit_size);							// store B
				memcpy(&pixels_[i], &pixels_[i + 2 * unit_size], unit_size);	// R to B
				memcpy(&pixels_[i + 2 * unit_size], temp, unit_size);			// restore B
			}
			delete[] temp;
		}
		u8* Image::Allocate(int w, int h, Format fmt)
		{
			width_ = w;
			height_ = h;
			format_ = fmt;
			int bpp = GetBpp(fmt);
			bpp_ = bpp >> 3; // bits to bytes
			pixels_ = new u8[width_ * height_ * bpp_];
			return pixels_;
		}
		bool Image::Save(const char* filename)
		{
			FileFormat fmt = ExtractFileFormat(filename);
			switch (fmt)
			{
			case Image::FileFormat::kBmp:
				return SaveBmp(filename);
			case Image::FileFormat::kJpg:
				return SaveJpeg(filename);
			case Image::FileFormat::kPng:
				return SavePng(filename);
			case Image::FileFormat::kTga:
				return SaveTga(filename);
			case Image::FileFormat::kTif:
				return SaveTiff(filename);
			case Image::FileFormat::kHdr:
				return SaveHdr(filename);
			default:
				assert(!"unknown image format");
				return false;
			}
		}
		bool Image::LoadFromFile(const char* filename)
		{
			FileFormat fmt = ExtractFileFormat(filename);
			switch (fmt)
			{
			case Image::FileFormat::kBmp:
				return LoadBmp(filename);
			case Image::FileFormat::kJpg:
				return LoadJpeg(filename);
			case Image::FileFormat::kPng:
				return LoadPng(filename);
			case Image::FileFormat::kTga:
				return LoadTga(filename);
			case Image::FileFormat::kTif:
				return LoadTiff(filename);
			case Image::FileFormat::kHdr:
				return LoadHdr(filename);
			default:
				assert(!"unknown image format");
				return false;
			}
		}
		bool Image::LoadCubemapFromFile(const char* filename, int ind)
		{
			if (!LoadFromFile(filename))
				return false;

			int w = width_ / 4;
			int h = height_ / 3;

			// texture offset by width and height in pixels
			int dw = 0, dh = 0;

			// pixels are stored from left bottom corner to right top
			switch (ind)
			{
			case 0: // +x front
				dw = w;
				dh = h;
				break;
			case 1: // -x back
				dw = 3 * w;
				dh = h;
				break;
			case 2: // +y up
				dw = 0;
				dh = 0;
				break;
			case 3: // -y down
				dw = 0;
				dh = 2 * h;
				break;
			case 4: // +z right
				dw = 0;
				dh = h;
				break;
			case 5: // -z left
				dw = 2 * w;
				dh = h;
				break;
			}

			u8 * new_pixels = new u8[w * h * bpp_];

			// copy pixel data
			for (int j = 0; j < h; j++)
				memcpy(&new_pixels[bpp_*(w*j)], &pixels_[bpp_*(width_*(dh + j) + dw)], bpp_*w); // copy line

			delete[] pixels_;

			// exchange loaded data for a part of it
			width_ = w;
			height_ = h;
			pixels_ = new_pixels;

			return true;
		}
		bool Image::LoadNMapFromHMap(const char* filename)
		{
			if (!LoadFromFile(filename))
				return false;

			int new_bpp = 3;
			u8 * new_pixels = new u8[width_ * height_ * new_bpp];

			if (data_type_ == DataType::kUint8) // standart bitmap
			{
				const float kOneOver255 = 1.0f / 255.0f;
                                int offs = 0;
				for (int i = 0; i < height_; i++)
					for (int j = 0; j < width_; j++)
					{
						float c  = (float)pixels_[(i*width_ + j)*channels_];
						float cx = (float)pixels_[(i*width_ + (j + 1) % width_)*channels_];
						float cy = (float)pixels_[(((i + 1) % height_)*width_ + j)*channels_];
						// convert height values to [0,1] range
						c  *= kOneOver255;
						cx *= kOneOver255;
						cy *= kOneOver255;

						// find derivatives
						float k = 3.0f;
						float dx = (c - cx) * k;
						float dy = (c - cy) * k;

						// normalize
						float len = sqrt(dx*dx + dy*dy + 1);

						// get normal
						float nx = dy / len;
						float ny = -dx / len;
						float nz = 1.0f / len;

						new_pixels[offs    ] = (u8)(128 + 127 * nx);
						new_pixels[offs + 1] = (u8)(128 + 127 * ny);
						new_pixels[offs + 2] = (u8)(128 + 127 * nz);
						offs += 3;
					}
			}
			else if (data_type_ == DataType::kFloat)
			{
                                int offs = 0;
				for (int i = 0; i < height_; i++)
					for (int j = 0; j < width_; j++)
					{
						float c  = reinterpret_cast<float*>(pixels_)[(i*width_ + j)*channels_];
						float cx = reinterpret_cast<float*>(pixels_)[(i*width_ + (j + 1) % width_)*channels_];
						float cy = reinterpret_cast<float*>(pixels_)[(((i + 1) % height_)*width_ + j)*channels_];

						// find derivatives
						float k = 3.0f;
						float dx = (c - cx) * k;
						float dy = (c - cy) * k;

						// normalize
						float len = sqrt(dx*dx + dy*dy + 1);

						// get normal
						float nx = dy / len;
						float ny = -dx / len;
						float nz = 1.0f / len;

						reinterpret_cast<float*>(new_pixels)[offs    ] = nx;
						reinterpret_cast<float*>(new_pixels)[offs + 1] = ny;
						reinterpret_cast<float*>(new_pixels)[offs + 2] = nz;
						offs += 3;
					}
			}
			else // assume its floating point format
			{
				assert(!"unknown data type");
				return false;
			}

			delete[] pixels_;
			pixels_ = new_pixels;
			channels_ = 3;
			data_type_ = DataType::kUint8;
			format_ = Format::kRGB8;

			return true;
		}
		bool Image::LoadNHMapFromHMap(const char* filename)
		{
			if (!LoadFromFile(filename))
				return false;

			int new_bpp = 4;
			u8 * new_pixels = new u8[width_ * height_ * new_bpp];

			if (data_type_ == DataType::kUint8) // standart bitmap
			{
				const float kOneOver255 = 1.0f / 255.0f;
                                int offs = 0;
				for (int i = 0; i < height_; i++)
					for (int j = 0; j < width_; j++)
					{
						float c  = (float)pixels_[(i*width_ + j)*channels_];
						float cx = (float)pixels_[(i*width_ + (j + 1) % width_)*channels_];
						float cy = (float)pixels_[(((i + 1) % height_)*width_ + j)*channels_];
						// convert height values to [0,1] range
						c  *= kOneOver255;
						cx *= kOneOver255;
						cy *= kOneOver255;

						// find derivatives
						float k = 3.0f;
						float dx = (c - cx) * k;
						float dy = (c - cy) * k;

						// normalize
						float len = sqrt(dx*dx + dy*dy + 1);

						// get normal
						float nx = dy / len;
						float ny = -dx / len;
						float nz = 1.0f / len;

						new_pixels[offs    ] = (unsigned char)(128 + 127 * nx);
						new_pixels[offs + 1] = (unsigned char)(128 + 127 * ny);
						new_pixels[offs + 2] = (unsigned char)(128 + 127 * nz);
						new_pixels[offs + 3] = (unsigned char)(255 * c);
						offs += 4;
					}
			}
			else if (data_type_ == DataType::kFloat)
			{
                                int offs = 0;
				for (int i = 0; i < height_; i++)
					for (int j = 0; j < width_; j++)
					{
						float c  = reinterpret_cast<float*>(pixels_)[(i*width_ + j)*channels_];
						float cx = reinterpret_cast<float*>(pixels_)[(i*width_ + (j + 1) % width_)*channels_];
						float cy = reinterpret_cast<float*>(pixels_)[(((i + 1) % height_)*width_ + j)*channels_];

						// find derivatives
						float k = 3.0f;
						float dx = (c - cx) * k;
						float dy = (c - cy) * k;

						// normalize
						float len = sqrt(dx*dx + dy*dy + 1);

						// get normal
						float nx = dy / len;
						float ny = -dx / len;
						float nz = 1.0f / len;

						reinterpret_cast<float*>(new_pixels)[offs    ] = nx;
						reinterpret_cast<float*>(new_pixels)[offs + 1] = ny;
						reinterpret_cast<float*>(new_pixels)[offs + 2] = nz;
						reinterpret_cast<float*>(new_pixels)[offs + 3] = c;
						offs += 4;
					}
			}
			else // assume its floating point format
			{
				assert(!"unknown data type");
				return false;
			}

			delete[] pixels_;
			pixels_ = new_pixels;
			channels_ = 4;
			data_type_ = DataType::kUint8;
			format_ = Format::kRGBA8;

			return true;
		}
		void Image::Rescale(int w, int h)
		{
			assert(!"TODO: Image::Rescale");
		}
		void Image::MakePowerOfTwo()
		{
			assert(!"TODO: Image::makePowerOfTwo");
		}

	} // namespace graphics
} // namespace sht