#include "../../include/image/image.h"
#include "../../../system/include/stream/file_stream.h"
#include "../../../system/include/stream/log_stream.h"
#include "../../../thirdparty/libpng/include/png.h"

#include <cstring>

namespace sht {
	namespace graphics {

		bool Image::SavePng(const char *filename)
		{
			// Get access to error log
			system::ErrorLogStream * error_log = system::ErrorLogStream::GetInstance();

            sht::system::FileStream stream;
            if (!stream.Open(filename, sht::system::StreamAccess::kWriteBinary))
			{
				error_log->PrintString("failed to open file '%s' for saving\n", filename);
				return false;
			}

			png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
			if (!png)
			{
				error_log->PrintString("png_create_write_struct failed during saving '%s'\n", filename);
				return false;
			}

			png_infop info = png_create_info_struct(png);
			if (!info)
			{
				error_log->PrintString("png_create_info_struct failed during saving '%s'\n", filename);
				png_destroy_write_struct(&png, NULL);
				return false;
			}

			if (setjmp(png_jmpbuf(png)))
			{
				error_log->PrintString("set_jmp failed during saving '%s'\n", filename);
				png_destroy_write_struct(&png, NULL);
				return false;
			}

			png_init_io(png, stream.GetFilePointer());

			// Output is 8bit depth, RGBA format.
			png_set_IHDR(
				png,
				info,
				width_, height_,
				8,
				PNG_COLOR_TYPE_RGBA,
				PNG_INTERLACE_NONE,
				PNG_COMPRESSION_TYPE_DEFAULT,
				PNG_FILTER_TYPE_DEFAULT
				);
			png_write_info(png, info);

			// To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
			// Use png_set_filler().
			//png_set_filler(png, 0, PNG_FILLER_AFTER);
			png_bytepp row_pointers = new png_bytep[height_];
			int row_stride = width_ * bpp_;
            if (inverted_row_order_)
            {
                for(int y = height_-1; y >= 0; --y)
                    row_pointers[y] = (png_bytep)(pixels_ + y*row_stride);
            }
            else // normal row order
            {
                for(int y = 0; y < height_; ++y)
                    row_pointers[y] = (png_bytep)(pixels_ + y*row_stride);
            }

			png_write_image(png, row_pointers);
			png_write_end(png, NULL);
			
			delete[] row_pointers;

			png_destroy_write_struct(&png, NULL);

            stream.Close();

			return true;
		}
		bool Image::LoadPng(const char *filename)
		{
			// Get access to error log
			system::ErrorLogStream * error_log = system::ErrorLogStream::GetInstance();

            sht::system::FileStream stream;
            if (!stream.Open(filename, sht::system::StreamAccess::kReadBinary))
            {
				error_log->PrintString("failed to open file '%s' for loading\n", filename);
                return false;
            }

			png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
			if (!png)
			{
				error_log->PrintString("png_create_read_struct failed during loading '%s'\n", filename);
				return false;
			}

			png_infop info = png_create_info_struct(png);
			if (!info)
			{
				error_log->PrintString("png_create_info_struct failed during loading '%s'\n", filename);
				png_destroy_read_struct(&png, NULL, NULL);
				return false;
			}

			if (setjmp(png_jmpbuf(png)))
			{
				error_log->PrintString("setjmp failed during loading '%s'\n", filename);
				png_destroy_read_struct(&png, NULL, NULL);
				return false;
			}

			png_init_io(png, stream.GetFilePointer());

			png_read_info(png, info);

			png_byte color_type = png_get_color_type(png, info);
			png_byte bit_depth  = png_get_bit_depth(png, info);

			// Read any color_type into 8bit depth, RGBA format.
			// See http://www.libpng.org/pub/png/libpng-manual.txt

			if (bit_depth == 16)
				png_set_strip_16(png);

			if (color_type == PNG_COLOR_TYPE_PALETTE)
				png_set_palette_to_rgb(png);

			// PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
			if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
				png_set_expand_gray_1_2_4_to_8(png);

			if (png_get_valid(png, info, PNG_INFO_tRNS))
				png_set_tRNS_to_alpha(png);

			// These color_type don't have an alpha channel then fill it with 0xff.
			if (color_type == PNG_COLOR_TYPE_RGB ||
				color_type == PNG_COLOR_TYPE_GRAY ||
				color_type == PNG_COLOR_TYPE_PALETTE)
				png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

			if (color_type == PNG_COLOR_TYPE_GRAY ||
				color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
				png_set_gray_to_rgb(png);

			png_read_update_info(png, info);
			
			width_      = png_get_image_width(png, info);
			height_     = png_get_image_height(png, info);
			channels_	= 4;
			data_type_ 	= DataType::kUint8;
			bpp_ 		= 4;
			format_ 	= Format::kRGBA8;

			size_t image_size = png_get_rowbytes(png, info) * height_;
			pixels_ = new u8[image_size];
			
			png_bytepp row_pointers = new png_bytep[height_];
			int row_stride = width_ * bpp_;
            
            if (inverted_row_order_)
            {
                for(int y = height_-1; y >= 0; --y)
                    row_pointers[y] = (png_bytep)(pixels_ + y*row_stride);
            }
            else // normal row order
            {
                for(int y = 0; y < height_; ++y)
                    row_pointers[y] = (png_bytep)(pixels_ + y*row_stride);
            }

			png_read_image(png, row_pointers);
			png_read_end(png, NULL);
			
			delete[] row_pointers;

			png_destroy_read_struct(&png, NULL, NULL);

            stream.Close();

			return true;
		}
		struct PngReadState {
			const u8 * buffer;
			size_t offset; // that we've read
			size_t size;
		};
		static void ReadDataFromBuffer(png_structp png_ptr, png_bytep data, png_size_t length)
		{
			PngReadState * state = (PngReadState *) png_get_io_ptr(png_ptr);
			if (state->offset + length <= state->size)
			{
				memcpy(data, state->buffer + state->offset, length);
				state->offset += length;
			}
			else
				png_error(png_ptr, "read error (ReadDataFromBuffer)");
		}
		bool Image::LoadFromBufferPng(const u8* buffer, size_t length)
		{
			// Get access to error log
			system::ErrorLogStream * error_log = system::ErrorLogStream::GetInstance();

			png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
			if (!png)
			{
				error_log->PrintString("png_create_read_struct failed\n");
				return false;
			}

			png_infop info = png_create_info_struct(png);
			if (!info)
			{
				error_log->PrintString("png_create_info_struct failed\n");
				png_destroy_read_struct(&png, NULL, NULL);
				return false;
			}

			if (setjmp(png_jmpbuf(png)))
			{
				error_log->PrintString("setjmp failed\n");
				png_destroy_read_struct(&png, NULL, NULL);
				return false;
			}

			PngReadState state;
			state.buffer = buffer;
			state.offset = 0;
			state.size = length;
			png_set_read_fn(png, &state, ReadDataFromBuffer);

			png_read_info(png, info);

			png_byte color_type = png_get_color_type(png, info);
			png_byte bit_depth = png_get_bit_depth(png, info);

			// Read any color_type into 8bit depth, RGBA format.
			// See http://www.libpng.org/pub/png/libpng-manual.txt

			if (bit_depth == 16)
				png_set_strip_16(png);

			if (color_type == PNG_COLOR_TYPE_PALETTE)
				png_set_palette_to_rgb(png);

			// PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
			if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
				png_set_expand_gray_1_2_4_to_8(png);

			if (png_get_valid(png, info, PNG_INFO_tRNS))
				png_set_tRNS_to_alpha(png);

			// These color_type don't have an alpha channel then fill it with 0xff.
			if (color_type == PNG_COLOR_TYPE_RGB ||
				color_type == PNG_COLOR_TYPE_GRAY ||
				color_type == PNG_COLOR_TYPE_PALETTE)
				png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

			if (color_type == PNG_COLOR_TYPE_GRAY ||
				color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
				png_set_gray_to_rgb(png);

			png_read_update_info(png, info);

			width_ = png_get_image_width(png, info);
			height_ = png_get_image_height(png, info);
			channels_ = 4;
			data_type_ = DataType::kUint8;
			bpp_ = 4;
			format_ = Format::kRGBA8;

			size_t image_size = png_get_rowbytes(png, info) * height_;
			pixels_ = new u8[image_size];

			png_bytepp row_pointers = new png_bytep[height_];
			int row_stride = width_ * bpp_;

			if (inverted_row_order_)
			{
				for (int y = height_ - 1; y >= 0; --y)
					row_pointers[y] = (png_bytep)(pixels_ + y*row_stride);
			}
			else // normal row order
			{
				for (int y = 0; y < height_; ++y)
					row_pointers[y] = (png_bytep)(pixels_ + y*row_stride);
			}

			png_read_image(png, row_pointers);
			png_read_end(png, NULL);

			delete[] row_pointers;

			png_destroy_read_struct(&png, NULL, NULL);

			return true;
		}

	} // namespace graphics
} // namespace sht