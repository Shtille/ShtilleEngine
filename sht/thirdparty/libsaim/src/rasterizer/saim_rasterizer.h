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

#ifndef __SAIM_RASTERIZER_H__
#define __SAIM_RASTERIZER_H__

#include "saim_bitmap_map.h"
#include "../saim_key_set.h"
#include "saim_bitmap_buffer.h"
#include "saim_data_pair_list.h"
#include "saim_bitmap_cache_info_list.h"

#include "../../deps/tinycthread.h"

#include <stdbool.h>

typedef struct {
	unsigned char* target_buffer;
	int target_width;
	int target_height;
	int target_bpp;

	mtx_t mutex;						//!< mutex to protect pending data
	saim_data_pair_list pending_data;   //!< data that pending for conversion
	saim_key_set requested_keys;           //!< to not make another calls if already requested
	saim_bitmap_map bitmap_map;         //!< extracted bitmaps
	int x_buffer_size;                  //!< size of x buffer
	int y_buffer_size;                  //!< size of y buffer
	int * x_keys;                       //!< transforms screen pixel x to key x
	int * x_samples;                    //!< transforms screen pixel x to sample x
	int * x_pixels;                     //!< transforms screen pixel x to bitmap pixel x
	int * y_keys;                       //!< transforms screen pixel y to key y
	int * y_samples;                    //!< transforms screen pixel y to sample y
	int * y_pixels;                     //!< transforms screen pixel y to bitmap pixel y
	saim_bitmap_buffer bitmap_buffer;   //!< buffer for fast access bitmap by key
	int render_counter;                 //!< counter for optimal cache sort
	int sort_shift_counter;             //!< counter to not do sort every frame
	saim_bitmap_cache_info_list bitmap_cache;
} saim_rasterizer;

bool saim_rasterizer__create(saim_rasterizer * rasterizer);
void saim_rasterizer__destroy(saim_rasterizer * rasterizer);

int saim_rasterizer__render_aligned(saim_rasterizer * rasterizer,
	double upper_latitude, double left_longitude, double lower_latitude, double right_longitude);
int saim_rasterizer__render_common(saim_rasterizer * rasterizer,
	double upper_latitude, double left_longitude, double lower_latitude, double right_longitude, float angle);

// Some function for inner use

void saim_rasterizer__push_request(saim_rasterizer * rasterizer, const saim_data_key * key);

//! Clear all the data
void saim_rasterizer__clear(saim_rasterizer * rasterizer);

const saim_bitmap * saim_rasterizer__get_bitmap(saim_rasterizer * rasterizer, const saim_data_key * key, bool use_counter);

//! Whether key been requested
bool saim_rasterizer__is_requested(saim_rasterizer * rasterizer, const saim_data_key * key);

//! Whether key been loaded
bool saim_rasterizer__is_loaded(saim_rasterizer * rasterizer, const saim_data_key * key);

void saim_rasterizer__add_request(saim_rasterizer * rasterizer, const saim_data_key * key);

//! Transform pending data to bitmap
void saim_rasterizer__data_transform(saim_rasterizer * rasterizer);

void saim_rasterizer__pre_render(saim_rasterizer * rasterizer,
	double upper_latitude, double left_longitude, double lower_latitude, double right_longitude,
	int* num_tiles_to_load, int* optimal_lod);
void saim_rasterizer__render_aligned_impl(saim_rasterizer * rasterizer,
	double upper_latitude, double left_longitude, double lower_latitude, double right_longitude,
	int level_of_detail);
void saim_rasterizer__render_common_impl(saim_rasterizer * rasterizer,
	double upper_latitude, double left_longitude, double lower_latitude, double right_longitude,
	int level_of_detail, float sin_angle, float cos_angle);

void saim_rasterizer__initialize_bitmap_buffer(saim_rasterizer * rasterizer, 
	int key_min_x, int key_max_x, int key_min_y, int key_max_y, int level_of_detail, bool is_break);
void saim_rasterizer__set_bitmap_for_buffer(saim_rasterizer * rasterizer, int key_x, int key_y, const saim_bitmap * bitmap);
void saim_rasterizer__set_bitmap_low_for_buffer(saim_rasterizer * rasterizer, int key_z_low, int key_x, int key_y, const saim_bitmap * bitmap);

void saim_rasterizer__clear_bitmap_buffer(saim_rasterizer * rasterizer);
const saim_buffered_bitmap_info * saim_rasterizer__get_bitmap_from_buffer(saim_rasterizer * rasterizer, int key_x, int key_y);
void saim_rasterizer__erase_key_from_buffer(saim_rasterizer * rasterizer, const saim_data_key * key);
void saim_rasterizer__sort_cache(saim_rasterizer * rasterizer);

#endif