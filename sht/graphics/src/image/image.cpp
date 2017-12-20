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
        static int GetChannels(Image::Format fmt)
        // Number of channels
        {
            switch (fmt)
            {
                case Image::Format::kA8:
                case Image::Format::kI8:
                case Image::Format::kL8:
                case Image::Format::kR8:
                case Image::Format::kA16:
                case Image::Format::kI16:
                case Image::Format::kL16:
                case Image::Format::kR16:
                case Image::Format::kA32:
                case Image::Format::kI32:
                case Image::Format::kL32:
                case Image::Format::kR32:
                    return 1;
                case Image::Format::kLA8:
                case Image::Format::kRG8:
                case Image::Format::kLA16:
                case Image::Format::kRG16:
                case Image::Format::kLA32:
                case Image::Format::kRG32:
                    return 2;
                case Image::Format::kRGB8:
                case Image::Format::kRGB16:
                case Image::Format::kRGB32:
                    return 3;
                case Image::Format::kRGBA8:
                case Image::Format::kRGBA16:
                case Image::Format::kRGBA32:
                    return 4;
                default:
                    assert(false && "unknown image format");
                    return 3;
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
			else if (ext == "hdr")
				return Image::FileFormat::kHdr;
			return Image::FileFormat::kUnknown;
		}
		Image::Image()
        : pixels_(nullptr)
        , inverted_row_order_(true)
		{
		}
		Image::Image(const Image& other)
		: pixels_(nullptr)
		, format_(other.format_)
		, data_type_(other.data_type_)
		, width_(other.width_)
		, height_(other.height_)
		, channels_(other.channels_)
		, bpp_(other.bpp_)
		, inverted_row_order_(other.inverted_row_order_)
		{
			size_t size = static_cast<size_t>(width_ * height_ * bpp_);
			pixels_ = new u8[size];
			memcpy(pixels_, other.pixels_, size);
		}
		Image::~Image()
		{
			if (pixels_) delete[] pixels_;
		}
		void Image::SetRowOrder(bool inverted)
		{
			inverted_row_order_ = inverted;
		}
		u8* Image::pixels()
		{
			return pixels_;
		}
		const u8* Image::pixels() const
		{
			return pixels_;
		}
		Image::Format Image::format() const
		{
			return format_;
		}
		int Image::width() const
		{
			return width_;
		}
		int Image::height() const
		{
			return height_;
		}
		int Image::bpp() const
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
            channels_ = GetChannels(fmt);
			if (pixels_) delete[] pixels_;
			pixels_ = new u8[width_ * height_ * bpp_];
			return pixels_;
		}
        void Image::FillWithZeroes()
        {
            memset(pixels_, 0, width_ * height_ * bpp_);
        }
		void Image::Copy(const Image& other)
		{
			if (pixels_) delete[] pixels_;
			size_t size = static_cast<size_t>(width_ * height_ * bpp_);
			pixels_ = new u8[size];
			memcpy(pixels_, other.pixels_, size);
			format_ = other.format_;
			data_type_ = other.data_type_;
			width_ = other.width_;
			height_ = other.height_;
			channels_ = other.channels_;
			bpp_ = other.bpp_;
			inverted_row_order_ = other.inverted_row_order_;
		}
        void Image::SubData(int offset_x, int offset_y, int w, int h, const u8* data)
        {
            int max_x = offset_x + w;
            int max_y = offset_y + h;
            assert(max_x <= width_);
            assert(max_y <= height_);
            
            // Data should have the same bpp as source
            for (int y = 0; y < h; ++y)
            {
                u8* dst = &pixels_[((y + offset_y)*width_ + offset_x)*bpp_];
                const u8* src = &data[(y*w)*bpp_];
                memcpy(dst, src, w * bpp_);
            }
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
		bool Image::LoadFromBuffer(const u8* buffer, size_t length)
		{
			// TODO: We need to recognize image type from buffer
			assert(false);
			FileFormat fmt = Image::FileFormat::kJpg;
			switch (fmt)
			{
			case Image::FileFormat::kBmp:
				return LoadFromBufferBmp(buffer, length);
			case Image::FileFormat::kJpg:
				return LoadFromBufferJpeg(buffer, length);
			case Image::FileFormat::kPng:
				return LoadFromBufferPng(buffer, length);
			case Image::FileFormat::kTga:
				return LoadFromBufferTga(buffer, length);
			case Image::FileFormat::kTif:
				return LoadFromBufferTiff(buffer, length);
			case Image::FileFormat::kHdr:
				return LoadFromBufferHdr(buffer, length);
			default:
				assert(!"unknown image format");
				return false;
			}
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

	} // namespace graphics
} // namespace sht