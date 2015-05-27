#include "../../include/image/image.h"
#include "../../../thirdparty/freeimage/include/freeimage.h"
#include <assert.h>
#include <math.h>
#include <memory.h>

namespace sht {
	namespace graphics {

//		static int GetChanels(Image::Format fmt)
//		{
//			switch (fmt)
//			{
//			case Image::Format::kRGBA8:
//			case Image::Format::kRGBA16:
//			case Image::Format::kRGBA32:
//				return 4;
//			case Image::Format::kRGB8:
//			case Image::Format::kRGB16:
//			case Image::Format::kRGB32:
//				return 3;
//			case Image::Format::kA8:
//			case Image::Format::kA16:
//			case Image::Format::kA32:
//			case Image::Format::kI8:
//			case Image::Format::kI16:
//			case Image::Format::kI32:
//			case Image::Format::kL8:
//			case Image::Format::kL16:
//			case Image::Format::kL32:
//			case Image::Format::kR8:
//			case Image::Format::kR16:
//			case Image::Format::kR32:
//				return 1;
//			case Image::Format::kRG8:
//			case Image::Format::kRG16:
//			case Image::Format::kRG32:
//			case Image::Format::kLA8:
//			case Image::Format::kLA16:
//			case Image::Format::kLA32:
//				return 2;
//			default:
//				assert(false && "unknown image format");
//				return 3;
//			}
//		}
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
		static FREE_IMAGE_FORMAT GetDataFormat(Image::DataType fmt)
		{
			switch (fmt)
			{
			case Image::DataType::kBmp: return FIF_BMP;
			case Image::DataType::kJpg: return FIF_JPEG;
			case Image::DataType::kPng: return FIF_PNG;
			case Image::DataType::kTga: return FIF_TARGA;
			case Image::DataType::kTif: return FIF_TIFF;
			default:
				assert(false && "unknown image format");
				return FIF_BMP;
			}
		}
		static FREE_IMAGE_TYPE GetDataType(Image::Format fmt)
		{
			switch (fmt)
			{
			case Image::Format::kA8:
			case Image::Format::kI8:
			case Image::Format::kL8:
			case Image::Format::kR8:
			case Image::Format::kLA8:
			case Image::Format::kRG8:
			case Image::Format::kRGB8:
			case Image::Format::kRGBA8:
				return FIT_BITMAP;
			case Image::Format::kA16:
			case Image::Format::kI16:
			case Image::Format::kL16:
			case Image::Format::kR16:
				return FIT_UINT16;
			case Image::Format::kA32:
			case Image::Format::kI32:
			case Image::Format::kL32:
			case Image::Format::kR32:
				return FIT_FLOAT;
			case Image::Format::kLA16: // ???
			case Image::Format::kRG16:
				return FIT_UINT32;
			case Image::Format::kLA32: // ???
			case Image::Format::kRG32:
				return FIT_DOUBLE;
			case Image::Format::kRGB16:
				return FIT_RGB16;
			case Image::Format::kRGB32:
				return FIT_RGBF;
			case Image::Format::kRGBA16:
				return FIT_RGBA16;
			case Image::Format::kRGBA32:
				return FIT_RGBAF;
			default:
				assert(false && "unknown image format");
				return FIT_BITMAP;
			}
		}
		static void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message)
		{
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
		u8* Image::Allocate(int w, int h, Format fmt)
		{
			width_ = w;
			height_ = h;
			format_ = fmt;
			int bpp = GetBpp(fmt);
			bpp_ = bpp >> 3; // bits to bytes
			FREE_IMAGE_TYPE type = GetDataType(fmt);
			FIBITMAP *hbmp = hbmp = FreeImage_AllocateT(type, w, h, bpp);
			pixels_ = (u8*)hbmp; // store freeimage bitmap object pointer
			return FreeImage_GetBits(hbmp);
		}
		void Image::Save(DataType fmt, const char* filename)
		{
			FIBITMAP *hbmp = (FIBITMAP*)pixels_; // restore pointer
			FreeImage_SetOutputMessage(FreeImageErrorHandler);
			try
			{
				FreeImage_Save(GetDataFormat(fmt), hbmp, filename);
			}
            catch(...)
            {
            }
            FreeImage_Unload(hbmp);
			pixels_ = nullptr;
		}
		bool Image::LoadFromFile(const char* filename)
		{
			FIBITMAP *hbmp;
			FREE_IMAGE_TYPE fit;
			BYTE *imageData;
			int imageSize, nChanels, unit_size, pitch;

			hbmp = FreeImage_Load(FreeImage_GetFileType(filename), filename);

			imageData = FreeImage_GetBits(hbmp);
			width_ = FreeImage_GetWidth(hbmp);
			height_ = FreeImage_GetHeight(hbmp);

			pitch = FreeImage_GetPitch(hbmp);
			bpp_ = FreeImage_GetBPP(hbmp) / 8; // convert bits to bytes; bpp = nChanels * unit_size
			imageSize = width_ * height_ * bpp_;

			fit = FreeImage_GetImageType(hbmp);

			switch (fit)
			{
			case FIT_BITMAP:
				unit_size = sizeof(u8);
				nChanels = bpp_ / unit_size;
				switch (nChanels)
				{
				case 4:
					format_ = Format::kRGBA8;
					break;
				case 3:
					format_ = Format::kRGB8;
					break;
				case 2:
					format_ = Format::kLA8;
					break;
				case 1:
					format_ = Format::kL8;
					break;
				}
				pixels_ = new u8[imageSize];
				break;
			case FIT_FLOAT:
				unit_size = sizeof(float);
				nChanels = 1;
				format_ = Format::kL32;
				pixels_ = new u8[imageSize];
				break;
			case FIT_RGBF:
				unit_size = sizeof(float);
				nChanels = 3;
				format_ = Format::kRGB32;
				pixels_ = new u8[imageSize];
				break;
			case FIT_RGBAF:
				unit_size = sizeof(float);
				nChanels = 4;
				format_ = Format::kRGBA32;
				pixels_ = new u8[imageSize];
				break;
			default: // FIT_UNKNOWN or other not supported
				assert(false && "unknown image file format");
				FreeImage_Unload(hbmp);
				return false;
			}

			// swap red and blue channels
			if (fit != FIT_RGBF)
			{
				u8 *temp = new u8[unit_size];
				for (int i = 0; i < width_*height_*bpp_; i += bpp_)
				{
					memcpy(temp, &imageData[i], unit_size); // store B
					memcpy(&imageData[i], &imageData[i + 2 * unit_size], unit_size); // R to B
					memcpy(&imageData[i + 2 * unit_size], temp, unit_size); // restore B
				}
				delete[] temp;
			}

			memcpy(pixels_, imageData, imageSize);

			FreeImage_Unload(hbmp);

			return true;
		}
		bool Image::LoadCubemapFromFile(const char* filename, int ind)
		{
			FIBITMAP *hbmp;
			FREE_IMAGE_TYPE fit;
			BYTE *imageData;
			int imageSize, nChanels, unit_size;

			hbmp = FreeImage_Load(FreeImage_GetFileType(filename), filename);

			imageData = FreeImage_GetBits(hbmp);
			width_ = FreeImage_GetWidth(hbmp);
			height_ = FreeImage_GetHeight(hbmp);

			int w = width_ / 4;
			int h = height_ / 3;

			bpp_ = FreeImage_GetBPP(hbmp) / 8; // convert bits to bytes; bpp = nChanels * unit_size
			imageSize = w*h*bpp_; // cuz 6 maps in a single file

			fit = FreeImage_GetImageType(hbmp);

			switch (fit)
			{
			case FIT_BITMAP:
				unit_size = sizeof(u8);
				nChanels = bpp_ / unit_size;
				switch (nChanels)
				{
				case 4:
					format_ = Format::kRGBA8;
					break;
				case 3:
					format_ = Format::kRGB8;
					break;
				case 2:
					format_ = Format::kLA8;
					break;
				case 1:
					format_ = Format::kL8;
					break;
				}
				pixels_ = new u8[imageSize];
				break;
			case FIT_FLOAT:
				unit_size = sizeof(float);
				nChanels = 1;
				format_ = Format::kL32;
				pixels_ = new u8[imageSize];
				break;
			case FIT_RGBF:
				unit_size = sizeof(float);
				nChanels = 3;
				format_ = Format::kRGB32;
				pixels_ = new u8[imageSize];
				break;
			case FIT_RGBAF:
				unit_size = sizeof(float);
				nChanels = 4;
				format_ = Format::kRGBA32;
				pixels_ = new u8[imageSize];
				break;
			default: // FIT_UNKNOWN or other not supported
				assert(false && "unknown image file format");
				FreeImage_Unload(hbmp);
				return false;
			}

			// swap red and blue channels
			if (fit != FIT_RGBF)
			{
				u8 *temp = new u8[unit_size];
				for (int i = 0; i < width_*height_*bpp_; i += bpp_)
				{
					memcpy(temp, &imageData[i], unit_size); // store B
					memcpy(&imageData[i], &imageData[i + 2 * unit_size], unit_size); // R to B
					memcpy(&imageData[i + 2 * unit_size], temp, unit_size); // restore B
				}
				delete[] temp;
			}

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

			// copy pixel data
			for (int j = 0; j < h; j++)
				memcpy(&pixels_[bpp_*(w*j)], &imageData[bpp_*(width_*(dh + j) + dw)], bpp_*w); // copy line

			width_ = w;
			height_ = h;

			FreeImage_Unload(hbmp);

			return true;
		}
		bool Image::LoadNMapFromHMap(const char* filename)
		{
			FIBITMAP *hbmp;
			FREE_IMAGE_TYPE fit;
			BYTE *imageData;
			int imageSize, nChanels, unit_size;

			hbmp = FreeImage_Load(FreeImage_GetFileType(filename), filename);

			imageData = FreeImage_GetBits(hbmp);
			width_ = FreeImage_GetWidth(hbmp);
			height_ = FreeImage_GetHeight(hbmp);
			bpp_ = FreeImage_GetBPP(hbmp) / 8; // convert bites to bytes
			imageSize = width_*height_ * 3; // new image size

			fit = FreeImage_GetImageType(hbmp);

			int offs = 0;
			switch (fit)
			{
			case FIT_BITMAP:
				format_ = Format::kRGB8;
				pixels_ = new unsigned char[imageSize];
				unit_size = sizeof(unsigned char);
				nChanels = bpp_ / unit_size;
				for (int i = 0; i < height_; i++)
					for (int j = 0; j < width_; j++)
					{
						float c = (float)imageData[(i*width_ + j)*nChanels];
						float cx = (float)imageData[(i*width_ + (j + 1) % width_)*nChanels];
						float cy = (float)imageData[(((i + 1) % height_)*width_ + j)*nChanels];
						// convert height values to [0,1] range
						float oneOver255 = 1.0f / 255.0f;
						c *= oneOver255;
						cx *= oneOver255;
						cy *= oneOver255;

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

						pixels_[offs] = (u8)(128 + 127 * nx);
						pixels_[offs + 1] = (u8)(128 + 127 * ny);
						pixels_[offs + 2] = (u8)(128 + 127 * nz);
						offs += 3;
					}
				break;
			case FIT_FLOAT:
			case FIT_RGBF:
			case FIT_RGBAF:
				format_ = Format::kRGB32;
				pixels_ = reinterpret_cast<u8*>(new float[imageSize]);
				unit_size = sizeof(float);
				nChanels = bpp_ / unit_size;
				for (int i = 0; i < height_; i++)
					for (int j = 0; j < width_; j++)
					{
						float c = reinterpret_cast<float*>(imageData)[(i*width_ + j)*nChanels];
						float cx = reinterpret_cast<float*>(imageData)[(i*width_ + (j + 1) % width_)*nChanels];
						float cy = reinterpret_cast<float*>(imageData)[(((i + 1) % height_)*width_ + j)*nChanels];

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

						reinterpret_cast<float*>(pixels_)[offs] = nx;
						reinterpret_cast<float*>(pixels_)[offs + 1] = ny;
						reinterpret_cast<float*>(pixels_)[offs + 2] = nz;
						offs += 3;
					}
				break;
			default: // FIT_UNKNOWN or other not supported
				FreeImage_Unload(hbmp);
				return false;
			}

			FreeImage_Unload(hbmp);

			return true;
		}
		bool Image::LoadNHMapFromHMap(const char* filename)
		{
			FIBITMAP *hbmp;
			FREE_IMAGE_TYPE fit;
			BYTE *imageData;
			int imageSize, nChanels, unit_size;

			hbmp = FreeImage_Load(FreeImage_GetFileType(filename), filename);

			imageData = FreeImage_GetBits(hbmp);
			width_ = FreeImage_GetWidth(hbmp);
			height_ = FreeImage_GetHeight(hbmp);
			bpp_ = FreeImage_GetBPP(hbmp) / 8; // convert bites to bytes
			imageSize = width_*height_ * 4; // new image size

			fit = FreeImage_GetImageType(hbmp);

			int offs = 0;
			switch (fit)
			{
			case FIT_BITMAP:
				format_ = Format::kRGBA8;
				pixels_ = new unsigned char[imageSize];
				unit_size = sizeof(unsigned char);
				nChanels = bpp_ / unit_size;
				for (int i = 0; i < height_; i++)
					for (int j = 0; j < width_; j++)
					{
						float c = (float)imageData[(i*width_ + j)*nChanels];
						float cx = (float)imageData[(i*width_ + (j + 1) % width_)*nChanels];
						float cy = (float)imageData[(((i + 1) % height_)*width_ + j)*nChanels];
						// convert height values to [0,1] range
						float oneOver255 = 1.0f / 255.0f;
						c *= oneOver255;
						cx *= oneOver255;
						cy *= oneOver255;

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

						pixels_[offs] = (unsigned char)(128 + 127 * nx);
						pixels_[offs + 1] = (unsigned char)(128 + 127 * ny);
						pixels_[offs + 2] = (unsigned char)(128 + 127 * nz);
						pixels_[offs + 3] = (unsigned char)(255 * c);
						offs += 4;
					}
				break;
			case FIT_FLOAT:
			case FIT_RGBF:
			case FIT_RGBAF:
				format_ = Format::kRGBA32;
				pixels_ = reinterpret_cast<unsigned char*>(new float[imageSize]);
				unit_size = sizeof(float);
				nChanels = bpp_ / unit_size;
				for (int i = 0; i < height_; i++)
					for (int j = 0; j < width_; j++)
					{
						float c = reinterpret_cast<float*>(imageData)[(i*width_ + j)*nChanels];
						float cx = reinterpret_cast<float*>(imageData)[(i*width_ + (j + 1) % width_)*nChanels];
						float cy = reinterpret_cast<float*>(imageData)[(((i + 1) % height_)*width_ + j)*nChanels];

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

						reinterpret_cast<float*>(pixels_)[offs] = nx;
						reinterpret_cast<float*>(pixels_)[offs + 1] = ny;
						reinterpret_cast<float*>(pixels_)[offs + 2] = nz;
						reinterpret_cast<float*>(pixels_)[offs + 3] = c;
						offs += 4;
					}
				break;
			default: // FIT_UNKNOWN or other not supported
				FreeImage_Unload(hbmp);
				return false;
			}

			FreeImage_Unload(hbmp);

			return true;
		}

	} // namespace graphics
} // namespace sht