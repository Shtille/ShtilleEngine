/*************************************************************************
* libsaim 1.0
* ------------------------------------------------------------------------
*  Copyright (c) 2016 Vladimir Sviridov <v.shtille@gmail.com>
* 
*  This software is provided 'as-is', without any express or implied
*  warranty. In no event will the authors be held liable for any damages
*  arising from the use of this software.
* 
*  Permission is granted to anyone to use this software for any purpose,
*  including commercial applications, and to alter it and redistribute it
*  freely, subject to the following restrictions:
* 
*  1. The origin of this software must not be misrepresented; you must not
*     claim that you wrote the original software. If you use this software
*     in a product, an acknowledgment in the product documentation would
*     be appreciated but is not required.
* 
*  2. Altered source versions must be plainly marked as such, and must not
*     be misrepresented as being the original software.
* 
*  3. This notice may not be removed or altered from any source
*     distribution.
* 
**************************************************************************/

#include "saim_decoder_png.h"

#include "../util/saim_memory.h"

#include "png.h"

#include <stdio.h>
#include <assert.h>

bool saim_decoder_png__check_signature(const unsigned char* buffer, size_t length)
{
	return png_check_sig(buffer, length);
}
bool saim_decoder_png__save(const char* filename, bool inverted_row_order,
	int width, int height, int bpp, const saim_bitmap * bitmap)
{
	assert(bpp == 4);

	FILE * file;
	file = fopen(filename, "wb");
	if (!file)
	{
		fprintf(stderr, "saim: can't open file '%s' for writing\n", filename);
		fclose(file);
		return false;
	}

	png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png)
	{
		fprintf(stderr, "saim: png_create_write_struct failed during saving '%s'\n", filename);
		fclose(file);
		return false;
	}

	png_infop info = png_create_info_struct(png);
	if (!info)
	{
		fprintf(stderr, "saim: png_create_info_struct failed during saving '%s'\n", filename);
		png_destroy_write_struct(&png, NULL);
		fclose(file);
		return false;
	}

	if (setjmp(png_jmpbuf(png)))
	{
		fprintf(stderr, "saim: set_jmp failed during saving '%s'\n", filename);
		png_destroy_write_struct(&png, NULL);
		fclose(file);
		return false;
	}

	png_init_io(png, file);

	// Output is 8bit depth, RGBA format.
	png_set_IHDR(
		png,
		info,
		width, height,
		8,
		PNG_COLOR_TYPE_RGBA,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT
		);
	png_write_info(png, info);

	unsigned char * src = bitmap->data;

	// To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
	// Use png_set_filler().
	//png_set_filler(png, 0, PNG_FILLER_AFTER);
	int row_stride = width * bpp;
	png_bytepp row_pointers = (png_bytepp) malloc(sizeof(png_bytep) * height);
	if (inverted_row_order)
	{
		for(int y = height-1; y >= 0; --y)
			row_pointers[y] = (png_bytep)(src + y * row_stride);
	}
	else // normal row order
	{
		for(int y = 0; y < height; ++y)
			row_pointers[y] = (png_bytep)(src + y * row_stride);
	}

	png_write_image(png, row_pointers);
	png_write_end(png, NULL);

	free(row_pointers);

	png_destroy_write_struct(&png, NULL);

	fclose(file);

	return true;
}
bool saim_decoder_png__load(const char* filename, bool inverted_row_order,
	int * width, int * height, int * bpp, saim_bitmap * bitmap)
{
	FILE * file;
	file = fopen(filename, "rb");
	if (!file)
	{
		fprintf(stderr, "saim: failed to open file '%s' for loading\n", filename);
	    return false;
	}

	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png)
	{
		fprintf(stderr, "saim: png_create_read_struct failed during loading '%s'\n", filename);
		fclose(file);
		return false;
	}

	png_infop info = png_create_info_struct(png);
	if (!info)
	{
		fprintf(stderr, "saim: png_create_info_struct failed during loading '%s'\n", filename);
		png_destroy_read_struct(&png, NULL, NULL);
		fclose(file);
		return false;
	}

	if (setjmp(png_jmpbuf(png)))
	{
		fprintf(stderr, "saim: setjmp failed during loading '%s'\n", filename);
		png_destroy_read_struct(&png, NULL, NULL);
		fclose(file);
		return false;
	}

	png_init_io(png, file);

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

	*width      = png_get_image_width(png, info);
	*height     = png_get_image_height(png, info);
	*bpp 		= 4;

	size_t image_size = png_get_rowbytes(png, info) * (*height);
	bitmap->data = SAIM_MALLOC(image_size);
	unsigned char * dst = bitmap->data;

	int row_stride = (*width) * (*bpp);
	png_bytepp row_pointers = (png_bytepp) malloc(sizeof(png_bytep)*(*height));

	if (inverted_row_order)
	{
	    for (int y = 0; y < (*height); ++y)
	        row_pointers[(*height)-1-y] = (png_bytep)(dst + y * row_stride);
	}
	else // normal row order
	{
	    for (int y = 0; y < (*height); ++y)
	        row_pointers[y] = (png_bytep)(dst + y * row_stride);
	}

	png_read_image(png, row_pointers);

	free(row_pointers);

	png_destroy_read_struct(&png, NULL, NULL);

	fclose(file);

	return true;
}
static void write_data_to_buffer(png_structp png_ptr, png_bytep in_bytes, png_size_t bytes_to_write)
{
	png_voidp io_ptr = png_get_io_ptr(png_ptr);
	if (io_ptr == NULL)
		return;
	// Use our own data source interface
	saim_string * data = (saim_string *) io_ptr;
	saim_string_append(data, (const char*)in_bytes, (unsigned int)bytes_to_write);
}
static void flush_function(png_structp png_ptr)
{

}
bool saim_decoder_png__save_to_buffer(const saim_bitmap * bitmap, bool inverted_row_order,
	int width, int height, int bpp, saim_string * data)
{
	png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png)
	{
		fprintf(stderr, "saim: png_create_write_struct failed\n");
		return false;
	}

	png_infop info = png_create_info_struct(png);
	if (!info)
	{
		fprintf(stderr, "saim: png_create_info_struct failed\n");
		png_destroy_write_struct(&png, NULL);
		return false;
	}

	if (setjmp(png_jmpbuf(png)))
	{
		fprintf(stderr, "saim: set_jmp failed\n");
		png_destroy_write_struct(&png, NULL);
		return false;
	}

	png_set_write_fn(png, data, write_data_to_buffer, flush_function);

	// Output is 8bit depth, RGBA format.
	png_set_IHDR(
		png,
		info,
		width, height,
		8,
		PNG_COLOR_TYPE_RGBA,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT
		);
	png_write_info(png, info);

	unsigned char * src = bitmap->data;

	// To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
	// Use png_set_filler().
	//png_set_filler(png, 0, PNG_FILLER_AFTER);
	int row_stride = width * bpp;
	png_bytepp row_pointers = (png_bytepp) malloc(sizeof(png_bytep) * height);
	if (inverted_row_order)
	{
		for(int y = height-1; y >= 0; --y)
			row_pointers[y] = (png_bytep)(src + y * row_stride);
	}
	else // normal row order
	{
		for(int y = 0; y < height; ++y)
			row_pointers[y] = (png_bytep)(src + y * row_stride);
	}

	png_write_image(png, row_pointers);
	png_write_end(png, NULL);

	free(row_pointers);

	png_destroy_write_struct(&png, NULL);

	return true;
}
// Simple struct that holds buffer offset
struct saim_png_state {
	const saim_string * data;
	size_t size; // number of bytes have been read
};
static void read_data_from_buffer(png_structp png_ptr, png_bytep out_bytes, png_size_t bytes_to_read)
{
	png_voidp io_ptr = png_get_io_ptr(png_ptr);
	if (io_ptr == NULL)
		return;
	// Use our own data source interface
	struct saim_png_state * state = (struct saim_png_state *) io_ptr;
	if (state->size + bytes_to_read <= state->data->length)
	{
		memcpy(out_bytes, state->data->data + state->size, bytes_to_read);
		state->size += bytes_to_read;
	}
	else
	{
		// Add some error handling here
	}
}
bool saim_decoder_png__load_from_buffer(const saim_string * data, bool inverted_row_order,
	int * width, int * height, int * bpp, saim_bitmap * bitmap)
{
	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png)
	{
		fprintf(stderr, "saim: png_create_read_struct failed\n");
		return false;
	}

	png_infop info = png_create_info_struct(png);
	if (!info)
	{
		fprintf(stderr, "saim: png_create_info_struct failed\n");
		png_destroy_read_struct(&png, NULL, NULL);
		return false;
	}

	if (setjmp(png_jmpbuf(png)))
	{
		fprintf(stderr, "saim: setjmp failed\n");
		png_destroy_read_struct(&png, NULL, NULL);
		return false;
	}

	struct saim_png_state state;
	state.data = data;
	state.size = 0;
	png_set_read_fn(png, &state, read_data_from_buffer);

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

	*width      = png_get_image_width(png, info);
	*height     = png_get_image_height(png, info);
	*bpp 		= 4;

	size_t image_size = png_get_rowbytes(png, info) * (*height);
	bitmap->data = SAIM_MALLOC(image_size);
	unsigned char * dst = bitmap->data;

	int row_stride = (*width) * (*bpp);
	png_bytepp row_pointers = (png_bytepp) malloc(sizeof(png_bytep)*(*height));

	if (inverted_row_order)
	{
	    for (int y = 0; y < (*height); ++y)
	        row_pointers[(*height)-1-y] = (png_bytep)(dst + y * row_stride);
	}
	else // normal row order
	{
	    for (int y = 0; y < (*height); ++y)
	        row_pointers[y] = (png_bytep)(dst + y * row_stride);
	}

	png_read_image(png, row_pointers);

	free(row_pointers);

	png_destroy_read_struct(&png, NULL, NULL);

	return true;
}