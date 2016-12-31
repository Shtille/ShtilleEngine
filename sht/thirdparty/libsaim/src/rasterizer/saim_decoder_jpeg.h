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

#ifndef __SAIM_DECODER_JPEG_H__
#define __SAIM_DECODER_JPEG_H__

#include "saim_bitmap.h"

#include "../util/saim_string.h"

#include <stdlib.h>
#include <stdbool.h>

// TODO
bool saim_decoder_jpeg__check_signature(const char* buffer, size_t length);

//
bool saim_decoder_jpeg__save(const char* filename, int quality, bool inverted_row_order,
	int width, int height, int bpp, const saim_bitmap * bitmap);

//
bool saim_decoder_jpeg__load(const char* filename, bool inverted_row_order,
	int * width, int * height, int * bpp, saim_bitmap * bitmap);

// Don't forget to call free(*dest_ptr) in the end
bool saim_decoder_jpeg__save_to_buffer(const saim_bitmap * bitmap, int quality, bool inverted_row_order,
	int width, int height, int bpp, unsigned char** dest_ptr, unsigned long* dest_size);

//
bool saim_decoder_jpeg__load_from_buffer(const saim_string * data, bool inverted_row_order,
	int * width, int * height, int * bpp, saim_bitmap * bitmap);

#endif