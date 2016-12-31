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

#ifndef __SAIM_BITMAP_BUFFER_H__
#define __SAIM_BITMAP_BUFFER_H__

#include "saim_bitmap.h"

#include <stdbool.h>

typedef struct {
	const saim_bitmap * bitmap;
	int level_ascending; // level - low_level, should be 0 or positive
} saim_buffered_bitmap_info;

//! Some struct to hold bitmap buffer data
typedef struct {
	saim_buffered_bitmap_info * data;
	int key_min_x;
	int key_min_y;
	int key_max_x;
	int key_max_y;
	int width;
	int level_of_detail; // base level of detail
	bool is_break;
	bool has_any_low_detailed_bitmap;
} saim_bitmap_buffer;

void saim_bitmap_buffer__create(saim_bitmap_buffer * buffer);
void saim_bitmap_buffer__destroy(saim_bitmap_buffer * buffer);

#endif