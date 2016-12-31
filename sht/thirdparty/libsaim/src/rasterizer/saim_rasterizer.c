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

#include "saim_rasterizer.h"

#include "saim_mercator.h"
#include "saim_decoder_jpeg.h"
#include "saim_decoder_png.h"
#include "../saim_provider.h"
#include "../saim_cache.h"

#include "../util/saim_memory.h"

#include <math.h>
#include <assert.h>

extern saim_provider * s_provider;
extern saim_cache * s_cache;
extern saim_rasterizer * s_rasterizer;

bool saim_rasterizer__create(saim_rasterizer * rasterizer)
{
	if (mtx_init(&rasterizer->mutex, mtx_plain) == thrd_error)
	{
		fprintf(stderr, "saim: mutex init failed\n");
		return false;
	}
	saim_data_pair_list__create(&rasterizer->pending_data);
	saim_key_set__create(&rasterizer->requested_keys);
	saim_bitmap_map__create(&rasterizer->bitmap_map);
	saim_bitmap_buffer__create(&rasterizer->bitmap_buffer);
	saim_bitmap_cache_info_list__create(&rasterizer->bitmap_cache);

	rasterizer->target_buffer = 0;
	rasterizer->x_buffer_size = 0;
	rasterizer->y_buffer_size = 0;
	rasterizer->x_keys = 0;
	rasterizer->x_samples = 0;
	rasterizer->x_pixels = 0;
	rasterizer->y_keys = 0;
	rasterizer->y_samples = 0;
	rasterizer->y_pixels = 0;
	rasterizer->render_counter = 0;
	rasterizer->sort_shift_counter = 0;

	return true;
}
void saim_rasterizer__destroy(saim_rasterizer * rasterizer)
{
	saim_rasterizer__clear(rasterizer);
	saim_bitmap_cache_info_list__destroy(&rasterizer->bitmap_cache);
	saim_bitmap_buffer__destroy(&rasterizer->bitmap_buffer);
	saim_bitmap_map__destroy(&rasterizer->bitmap_map);
	saim_key_set__destroy(&rasterizer->requested_keys);
	saim_data_pair_list__destroy(&rasterizer->pending_data);
	mtx_destroy(&rasterizer->mutex);
	if (rasterizer->x_buffer_size)
	{
		SAIM_FREE(rasterizer->x_keys);
		SAIM_FREE(rasterizer->x_samples);
		SAIM_FREE(rasterizer->x_pixels);
	}
	if (rasterizer->y_buffer_size)
	{
		SAIM_FREE(rasterizer->y_keys);
		SAIM_FREE(rasterizer->y_samples);
		SAIM_FREE(rasterizer->y_pixels);
	}
}
int saim_rasterizer__render_aligned(saim_rasterizer * rasterizer,
	double upper_latitude, double left_longitude, double lower_latitude, double right_longitude)
{
	int num_tiles_to_load, optimal_lod;
	if (rasterizer->target_buffer == 0)
	{
		fprintf(stderr, "saim: target buffer hasn't been set\n");
		return -1;
	}
	saim_rasterizer__pre_render(rasterizer,
		upper_latitude, left_longitude, lower_latitude, right_longitude,
		&num_tiles_to_load, &optimal_lod);
	saim_rasterizer__render_aligned_impl(rasterizer,
		upper_latitude, left_longitude, lower_latitude, right_longitude, optimal_lod);
	return num_tiles_to_load;
}
int saim_rasterizer__render_common(saim_rasterizer * rasterizer,
	double upper_latitude, double left_longitude, double lower_latitude, double right_longitude, float angle)
{
	int num_tiles_to_load, optimal_lod;
	float sin_angle, cos_angle;
	const float kDegToRad = 0.017453292f;
	const float angle_rad = angle * kDegToRad;
	if (rasterizer->target_buffer == 0)
	{
		fprintf(stderr, "saim: target buffer hasn't been set\n");
		return -1;
	}
	sin_angle = sinf(angle_rad);
	cos_angle = cosf(angle_rad);
	saim_rasterizer__pre_render(rasterizer,
		upper_latitude, left_longitude, lower_latitude, right_longitude,
		&num_tiles_to_load, &optimal_lod);
	saim_rasterizer__render_common_impl(rasterizer,
		upper_latitude, left_longitude, lower_latitude, right_longitude, optimal_lod, sin_angle, cos_angle);
	return num_tiles_to_load;
}

// -------------------- Internal use only functions -------------------------

static void tile_notification_function(const saim_data_key * key, saim_string * data, bool success)
{
	saim_data_pair * pair;

	pair = (saim_data_pair *) SAIM_MALLOC(sizeof(saim_data_pair));
	saim_data_pair__create(pair);
	
	mtx_lock(&s_rasterizer->mutex);
	pair->key = *key; // copy key value
	if (success)
	{
		saim_string_swap(&pair->data, data);
	}
	saim_data_pair_list__push_back(&s_rasterizer->pending_data, pair);
	mtx_unlock(&s_rasterizer->mutex);
}

void saim_rasterizer__push_request(saim_rasterizer * rasterizer, const saim_data_key * key)
{
	// Whether key requested to load or loaded
    if (!saim_rasterizer__is_requested(rasterizer, key))
    {
        // Add key to requested list
        saim_rasterizer__add_request(rasterizer, key);

        // Perform query for each key that hasn't been loaded
        saim_cache__tile_service_load_query(s_cache, key, tile_notification_function);
    }
}
void saim_rasterizer__clear(saim_rasterizer * rasterizer)
{
	mtx_lock(&rasterizer->mutex);
	saim_data_pair_list__clear(&rasterizer->pending_data);
	mtx_unlock(&rasterizer->mutex);

	saim_key_set__clear(&rasterizer->requested_keys);
	saim_bitmap_map__clear(&rasterizer->bitmap_map);
	saim_bitmap_cache_info_list__clear(&rasterizer->bitmap_cache);
}
const saim_bitmap * saim_rasterizer__get_bitmap(saim_rasterizer * rasterizer, const saim_data_key * key, bool use_counter)
{
	saim_set_node * node;
	saim_bitmap_info_pair * pair;
	node = saim_bitmap_map__search(&rasterizer->bitmap_map, key);
	if (node != rasterizer->bitmap_map.set->nil)
	{
		pair = (saim_bitmap_info_pair *) node->data;
		if (use_counter)
			*pair->info.p_usage = rasterizer->render_counter; // use render counter as bitmap existence time
		return &(pair->info.bitmap);
	}
	else
		return NULL;
}
bool saim_rasterizer__is_requested(saim_rasterizer * rasterizer, const saim_data_key * key)
{
	saim_set_node * node;
	node = saim_key_set__search(&rasterizer->requested_keys, key);
	return node != rasterizer->requested_keys.set->nil;
}
bool saim_rasterizer__is_loaded(saim_rasterizer * rasterizer, const saim_data_key * key)
{
	saim_set_node * node;
	node = saim_bitmap_map__search(&rasterizer->bitmap_map, key);
	return node != rasterizer->bitmap_map.set->nil;
}
void saim_rasterizer__add_request(saim_rasterizer * rasterizer, const saim_data_key * key)
{
	saim_data_key * key_copy;
	key_copy = (saim_data_key *)SAIM_MALLOC(sizeof(saim_data_key));
	saim_data_key__set_by_other(key_copy, key); // copy
	saim_key_set__insert(&rasterizer->requested_keys, key_copy);
}
static void empty_image(saim_rasterizer * rasterizer, saim_bitmap * bitmap)
{
	// Make data the same bitness as destination bitmap
	const int kBitmapWidth = s_provider->bitmap_width;
	const int kBitmapHeight = s_provider->bitmap_height;
	const int screen_bpp = rasterizer->target_bpp;
	const int bitmap_size = kBitmapWidth * kBitmapHeight * screen_bpp;
	unsigned char * data;

	saim_bitmap__destroy(bitmap);
	saim_bitmap__create(bitmap);
	bitmap->data = (unsigned char*) SAIM_MALLOC(bitmap_size);
	data = bitmap->data;
	switch (screen_bpp)
	{
	case 1:
		for (int i = 0; i < bitmap_size; i += screen_bpp)
		{
			*data++ = 0xFF;
		}
		break;
	case 2:
		for (int i = 0; i < bitmap_size; i += screen_bpp)
		{
			// TODO: RGB(1,1,1) to R5G6B5
			*data++ = 0xFF;
			*data++ = 0xFF;
		}
		break;
	case 3:
		for (int i = 0; i < bitmap_size; i += screen_bpp)
		{
			*data++ = 0xFF;
			*data++ = 0xFF;
			*data++ = 0xFF;
		}
		break;
	case 4:
		for (int i = 0; i < bitmap_size; i += screen_bpp)
		{
			*data++ = 0xFF;
			*data++ = 0xFF;
			*data++ = 0xFF;
			*data++ = 0xFF;
		}
		break;
	default:
		assert(false && "wrong number of bytes per pixel");
	}
}
static bool decode_png(saim_rasterizer * rasterizer, const saim_string * data, saim_bitmap * bitmap)
{
	// Width, height and bpp parameters are known
	int width, height, bpp;
	saim_decoder_png__load_from_buffer(data, false, &width, &height, &bpp, bitmap);
	assert(width == s_provider->bitmap_width);
	assert(height == s_provider->bitmap_height);
	assert(bpp == s_rasterizer->target_bpp);
	return true;
}
static bool decode_jpg(saim_rasterizer * rasterizer, const saim_string * data, saim_bitmap * bitmap)
{
	// Width, height and bpp parameters are known
	int width, height, bpp;
	saim_decoder_jpeg__load_from_buffer(data, false, &width, &height, &bpp, bitmap);
	assert(width == s_provider->bitmap_width);
	assert(height == s_provider->bitmap_height);
	assert(bpp == s_rasterizer->target_bpp);
	return true;
}
static void decode_image(saim_rasterizer * rasterizer, const saim_string * data, saim_bitmap * bitmap)
{
	// If we try to decode wrong type of image, our app will crash
	// So we have to recognize the image type
	if (data->length < 3) // number of bytes to decode 'PNG' tag
	{
		empty_image(rasterizer, bitmap);
		return;
	}
	if (data->data[1] == 'P' &&
		data->data[2] == 'N' &&
		data->data[3] == 'G')
	{
		// Format is PNG
		if (!decode_png(rasterizer, data, bitmap))
		{
			fprintf(stderr, "saim: PNG image decode error\n");
			empty_image(rasterizer, bitmap);
		}
	}
	else
	{
		// Format is JPG
		if (!decode_jpg(rasterizer, data, bitmap))
		{
			fprintf(stderr, "saim: JPG image decode error\n");
			empty_image(rasterizer, bitmap);
		}
	}
}
void saim_rasterizer__data_transform(saim_rasterizer * rasterizer)
{
	saim_data_pair * pair;
	const saim_data_key * key;
	saim_set_node * node;
	saim_bitmap_cache_info * info;
	saim_bitmap_info_pair * info_pair;
	saim_bitmap * bitmap;

	++rasterizer->render_counter;

	// Perform cache sort once per 10 frames
	++rasterizer->sort_shift_counter;
	if (rasterizer->sort_shift_counter == 10)
	{
		rasterizer->sort_shift_counter = 0;
		saim_rasterizer__sort_cache(rasterizer);
	}

	for (;;)
	{
		mtx_lock(&rasterizer->mutex);
		pair = saim_data_pair_list__pop_front(&rasterizer->pending_data);
		mtx_unlock(&rasterizer->mutex);
		if (!pair)
			break; // there is no data to transform

		key = &pair->key;

		// Check for skipped data
		if (pair->data.length != 0)
		{
			const unsigned int kBitmapCapacity = 500;
			if (saim_bitmap_map__size(&rasterizer->bitmap_map) == kBitmapCapacity)
			{
				// Our cache is full, so we gonna replace existing bitmaps
				info = saim_bitmap_cache_info_list__pop_front(&rasterizer->bitmap_cache);
				saim_rasterizer__erase_key_from_buffer(rasterizer, &info->key);
				node = saim_bitmap_map__search(&rasterizer->bitmap_map, &info->key);
				saim_bitmap_map__erase(&rasterizer->bitmap_map, node);
				SAIM_FREE(info);
            }

			// Insert a bitmap into our map
            info = (saim_bitmap_cache_info *) SAIM_MALLOC(sizeof(saim_bitmap_cache_info));
            // saim_bitmap_cache_info is a simple struct
            info->key = *key;
            info->usage = rasterizer->render_counter;
            saim_bitmap_cache_info_list__push_back(&rasterizer->bitmap_cache, info);
            info_pair = (saim_bitmap_info_pair *) SAIM_MALLOC(sizeof(saim_bitmap_info_pair));
            saim_bitmap_info_pair__create(info_pair);
            info_pair->key = *key;
            node = saim_bitmap_map__insert(&rasterizer->bitmap_map, info_pair);
            info_pair->info.p_usage = & info->usage;
            bitmap = & info_pair->info.bitmap;

			// Pair stores encoded image data, so we have to decode it before use
			decode_image(rasterizer, &pair->data, bitmap);
		}

		// Data is loaded, so no we don't need this key in requested list
		node = saim_key_set__search(&rasterizer->requested_keys, key);
		saim_key_set__erase(&rasterizer->requested_keys, node);

		// Finally
		saim_data_pair__destroy(pair);
		SAIM_FREE(pair);
	}
}
void saim_rasterizer__pre_render(saim_rasterizer * rasterizer,
	double upper_latitude, double left_longitude, double lower_latitude, double right_longitude,
	int* num_tiles_to_load, int* optimal_lod)
{
	int left, right, top, bottom;
	saim_data_key key, key_low;
	const saim_bitmap * bitmap, * bitmap_low;

	double screen_pixel_size_x = (right_longitude - left_longitude)/(double)rasterizer->target_width;
    *optimal_lod = saim_get_optimal_level_of_detail(screen_pixel_size_x);

    // Compute tile keys for bound rect
    saim_lat_long_to_tile_xy(upper_latitude, saim_normalized_longitude(left_longitude),
		*optimal_lod, &left, &top);
    saim_lat_long_to_tile_xy(lower_latitude, saim_normalized_longitude(right_longitude),
		*optimal_lod, &right, &bottom);

    int tiles_per_side = 1 << *optimal_lod;

    // Calculate number of tiles (bitmaps) in longitude (x) direction
    int num_x = right - left + 1;
    bool has_break = (num_x <= 0);
    if (has_break)
        num_x += tiles_per_side;

    saim_rasterizer__initialize_bitmap_buffer(rasterizer, left, right, top, bottom, *optimal_lod, has_break);

    // Transform rect to list of keys
    *num_tiles_to_load = 0;

    for (int y = top; y <= bottom; ++y)
    for (int i = 0; i < num_x; ++i)
    {
        int x = (left + i) % tiles_per_side;
        // Add a key to the list
        saim_data_key__create(&key, x, y, *optimal_lod);
        bitmap = saim_rasterizer__get_bitmap(rasterizer, &key, true);

        if (bitmap == NULL) // not loaded yet
        {
            ++*num_tiles_to_load;

            saim_rasterizer__push_request(rasterizer, &key);

            // Try to use low detailed version of bitmap when it didn't load
            bool bitmap_found = false;
            bool lazy_bitmap_found = false;
            for (int i = 1; i <= *optimal_lod - s_provider->min_lod; ++i)
            {
                int lod = *optimal_lod - i;
                int key_x_low = x >> i;
                int key_y_low = y >> i;
                saim_data_key__create(&key_low, key_x_low, key_y_low, lod);
                bitmap_low = saim_rasterizer__get_bitmap(rasterizer, &key_low, false);
                if (bitmap_low)
                {
                    bitmap_found = true;
                    saim_rasterizer__set_bitmap_low_for_buffer(rasterizer, lod, x, y, bitmap_low);
                    break;
                }
                else
                {
                    // Poor internet fix to push low detailed cached bitmaps to load
                    if (!lazy_bitmap_found && saim_cache__is_exist(s_cache, &key_low))
                    {
                        lazy_bitmap_found = true;
                        saim_rasterizer__push_request(rasterizer, &key_low);
                    }
                }
            }
            
            // Otherwise nothing to render
            if (!bitmap_found)
                saim_rasterizer__set_bitmap_for_buffer(rasterizer, x, y, NULL);
        }
        else
        {
            saim_rasterizer__set_bitmap_for_buffer(rasterizer, x, y, bitmap);
        }
    }

    // Transform any pending data
    saim_rasterizer__data_transform(rasterizer);
}
void saim_rasterizer__render_aligned_impl(saim_rasterizer * rasterizer,
	double upper_latitude,
	double left_longitude,
	double lower_latitude,
	double right_longitude,
	int level_of_detail)
{
	// Bitmap should have the same BPP as the target buffer
	unsigned char * dst = rasterizer->target_buffer;

	const int screen_width = rasterizer->target_width;
	const int screen_height = rasterizer->target_height;
	const int screen_bpp = rasterizer->target_bpp;
	const int kBitmapWidth = s_provider->bitmap_width;
	const int kBitmapHeight = s_provider->bitmap_height;

	int map_size = kBitmapWidth << level_of_detail;

	// Compute pixel sizes
	double screen_pixel_size_x = (right_longitude - left_longitude)/(double)screen_width;
	double screen_pixel_size_y = (upper_latitude - lower_latitude)/(double)screen_height;

	// Doubled size of maximum dimension done due to rotation perspective
	const int total_buffer_size = (screen_width > screen_height) ? (screen_width << 1) : (screen_height << 1);

	// Resize buffers
	if (rasterizer->x_buffer_size != total_buffer_size)
	{
		if (rasterizer->x_buffer_size)
		{
			SAIM_FREE(rasterizer->x_keys);
			SAIM_FREE(rasterizer->x_samples);
			SAIM_FREE(rasterizer->x_pixels);
		}
		rasterizer->x_buffer_size = total_buffer_size;
		rasterizer->x_keys    = (int*) SAIM_MALLOC(rasterizer->x_buffer_size * sizeof(int));
		rasterizer->x_samples = (int*) SAIM_MALLOC(rasterizer->x_buffer_size * sizeof(int));
		rasterizer->x_pixels  = (int*) SAIM_MALLOC(rasterizer->x_buffer_size * sizeof(int));
	}
	if (rasterizer->y_buffer_size != total_buffer_size)
	{
		if (rasterizer->y_buffer_size)
		{
			SAIM_FREE(rasterizer->y_keys);
			SAIM_FREE(rasterizer->y_samples);
			SAIM_FREE(rasterizer->y_pixels);
		}
		rasterizer->y_buffer_size = total_buffer_size;
		rasterizer->y_keys    = (int*) SAIM_MALLOC(rasterizer->y_buffer_size * sizeof(int));
		rasterizer->y_samples = (int*) SAIM_MALLOC(rasterizer->y_buffer_size * sizeof(int));
		rasterizer->y_pixels  = (int*) SAIM_MALLOC(rasterizer->y_buffer_size * sizeof(int));
	}

	// Copy pointers to variables for faster access
	int * x_keys    = rasterizer->x_keys;
	int * x_samples = rasterizer->x_samples;
	int * x_pixels  = rasterizer->x_pixels;
	int * y_keys    = rasterizer->y_keys;
	int * y_samples = rasterizer->y_samples;
	int * y_pixels  = rasterizer->y_pixels;

	// Fill buffers
	for (int x = 0; x < screen_width; ++x)
	{
		double pixel_lon = saim_normalized_longitude(left_longitude + screen_pixel_size_x * ((double)x + 0.5));
		int bitmap_x;
		saim_longitude_to_pixel_x(pixel_lon, map_size, &bitmap_x);
		x_keys[x]    = bitmap_x / kBitmapWidth;
		x_samples[x] = bitmap_x % kBitmapWidth;
		x_pixels[x]  = bitmap_x;
	}
	for (int y = 0; y < screen_height; ++y)
	{
		double pixel_lat = upper_latitude - screen_pixel_size_y * ((double)y + 0.5);
		int bitmap_y;
		saim_latitude_to_pixel_y(pixel_lat, map_size, &bitmap_y);
		y_keys[y]    = bitmap_y / kBitmapHeight;
		y_samples[y] = bitmap_y % kBitmapHeight;
		y_pixels[y]  = bitmap_y;
	}

	int dst_index = 0;
	for (int y = 0; y < screen_height; ++y)
	{
		int key_y = y_keys[y];
		int sample_y_dot_width = y_samples[y] * kBitmapWidth;
		for (int x = 0; x < screen_width; ++x)
		{
			int key_x = x_keys[x];

			const saim_buffered_bitmap_info * info;
			info = saim_rasterizer__get_bitmap_from_buffer(rasterizer, key_x, key_y);
			if (info->bitmap == NULL)
			{
				// Tile hasn't been loaded yet
				dst_index += screen_bpp;
				continue;
			}

			unsigned char * src = info->bitmap->data;
			assert(src);

			int src_index;
			if (info->level_ascending == 0)
			{
				int sample_x = x_samples[x];
				src_index = sample_y_dot_width + sample_x;
			}
			else
			{
				int bitmap_x = x_pixels[x];
				int bitmap_y = y_pixels[y];
				int bitmap_x_low = bitmap_x >> info->level_ascending;
				int sample_x_low = bitmap_x_low % kBitmapWidth;
				int bitmap_y_low = bitmap_y >> info->level_ascending;
				int sample_y_low = bitmap_y_low % kBitmapHeight;
				src_index = sample_y_low * kBitmapWidth + sample_x_low;
			}
			src_index *= screen_bpp;

			// Formula for pixel offset is pixel[(y * width + x) * bpp]
			memcpy(dst + dst_index, src + src_index, screen_bpp);

			dst_index += screen_bpp;
		} // for x
	} // for y
}
void saim_rasterizer__render_common_impl(saim_rasterizer * rasterizer,
	double upper_latitude, double left_longitude, double lower_latitude, double right_longitude,
	int level_of_detail, float sin_angle, float cos_angle)
{
	unsigned char * dst = rasterizer->target_buffer;

	const int screen_width = rasterizer->target_width;
	const int screen_height = rasterizer->target_height;
	const int screen_bpp = rasterizer->target_bpp;
	const int kBitmapWidth = s_provider->bitmap_width;
	const int kBitmapHeight = s_provider->bitmap_height;

	int map_size = kBitmapWidth << level_of_detail;

	float frotated_screen_width = fabsf((float)screen_width * cos_angle) +
								  fabsf((float)screen_height * sin_angle);
	float frotated_screen_height = fabsf((float)screen_width * sin_angle) +
								   fabsf((float)screen_height * cos_angle);
	int rotated_screen_width = (int)(frotated_screen_width);
	int rotated_screen_height = (int)(frotated_screen_height);

	// Compute pixel sizes
	double screen_pixel_size_x = (right_longitude - left_longitude)/frotated_screen_width;
	double screen_pixel_size_y = (upper_latitude - lower_latitude)/frotated_screen_height;

	// We gonna rotate our screen, so maximum size will be sqrt(w^2 + h^2)
	// But we gonna allocate max(w,h)*2 that is more than enough
	const int total_buffer_size = (screen_width > screen_height) ? (screen_width << 1) : (screen_height << 1);

	// Resize buffers
	if (rasterizer->x_buffer_size != total_buffer_size)
	{
		if (rasterizer->x_buffer_size)
		{
			SAIM_FREE(rasterizer->x_keys);
			SAIM_FREE(rasterizer->x_samples);
			SAIM_FREE(rasterizer->x_pixels);
		}
		rasterizer->x_buffer_size = total_buffer_size;
		rasterizer->x_keys    = (int*) SAIM_MALLOC(rasterizer->x_buffer_size * sizeof(int));
		rasterizer->x_samples = (int*) SAIM_MALLOC(rasterizer->x_buffer_size * sizeof(int));
		rasterizer->x_pixels  = (int*) SAIM_MALLOC(rasterizer->x_buffer_size * sizeof(int));
	}
	if (rasterizer->y_buffer_size != total_buffer_size)
	{
		if (rasterizer->y_buffer_size)
		{
			SAIM_FREE(rasterizer->y_keys);
			SAIM_FREE(rasterizer->y_samples);
			SAIM_FREE(rasterizer->y_pixels);
		}
		rasterizer->y_buffer_size = total_buffer_size;
		rasterizer->y_keys    = (int*) SAIM_MALLOC(rasterizer->y_buffer_size * sizeof(int));
		rasterizer->y_samples = (int*) SAIM_MALLOC(rasterizer->y_buffer_size * sizeof(int));
		rasterizer->y_pixels  = (int*) SAIM_MALLOC(rasterizer->y_buffer_size * sizeof(int));
	}

	// Copy pointers to variables for faster access
	int * x_keys    = rasterizer->x_keys;
	int * x_samples = rasterizer->x_samples;
	int * x_pixels  = rasterizer->x_pixels;
	int * y_keys    = rasterizer->y_keys;
	int * y_samples = rasterizer->y_samples;
	int * y_pixels  = rasterizer->y_pixels;

	// Fill buffers
	for (int x = 0; x < rotated_screen_width; ++x)
	{
		double pixel_lon = saim_normalized_longitude(left_longitude + screen_pixel_size_x * ((double)x + 0.5));
		int bitmap_x;
		saim_longitude_to_pixel_x(pixel_lon, map_size, &bitmap_x);
		x_keys[x]    = bitmap_x / kBitmapWidth;
		x_samples[x] = bitmap_x % kBitmapWidth;
		x_pixels[x]  = bitmap_x;
	}
	for (int y = 0; y < rotated_screen_height; ++y)
	{
		double pixel_lat = upper_latitude - screen_pixel_size_y * ((double)y + 0.5);
		int bitmap_y;
		saim_latitude_to_pixel_y(pixel_lat, map_size, &bitmap_y);
		y_keys[y]    = bitmap_y / kBitmapHeight;
		y_samples[y] = bitmap_y % kBitmapHeight;
		y_pixels[y]  = bitmap_y;
	}

	float x0 = (frotated_screen_width/2.0f)
		- (float)(screen_width/2)  * cos_angle
		+ (float)(screen_height/2) * sin_angle;
	float y0 = (frotated_screen_height/2.0f)
		- (float)(screen_width/2)  * sin_angle
		- (float)(screen_height/2) * cos_angle;

	int dst_index = 0;
	float xtr, ytr;
	for (int y = 0; y < screen_height; ++y)
	{
		float yinv = (float)(y);
		xtr = x0 - yinv * sin_angle;
		ytr = y0 + yinv * cos_angle;
		for (int x = 0; x < screen_width; ++x)
		{
			//float xtr = x0 + xinv * cos_angle - yinv * sin_angle;
			//float ytr = y0 + xinv * sin_angle + yinv * cos_angle;
			int x_mapped = (int)(xtr);
			int y_mapped = (int)(ytr);
			xtr += cos_angle;
			ytr += sin_angle;
			// Clamp values to not cause exceed buffer size
			if (x_mapped < 0) x_mapped = 0;
			if (y_mapped < 0) y_mapped = 0;
			if (x_mapped >= rotated_screen_width) x_mapped = rotated_screen_width-1;
			if (y_mapped >= rotated_screen_height) y_mapped = rotated_screen_height-1;

			int key_x = x_keys[x_mapped];
			int key_y = y_keys[y_mapped];

			const saim_buffered_bitmap_info * info;
			info = saim_rasterizer__get_bitmap_from_buffer(rasterizer, key_x, key_y);
			if (info->bitmap == NULL)
			{
				// Tile hasn't been loaded yet
				dst_index += screen_bpp;
				continue;
			}

			unsigned char * src = info->bitmap->data;
			assert(src);

			int src_index;
			if (info->level_ascending == 0)
			{
				int sample_x = x_samples[x_mapped];
				int sample_y = y_samples[y_mapped];
				src_index = sample_y * kBitmapWidth + sample_x;
			}
			else
			{
				int bitmap_x = x_pixels[x_mapped];
				int bitmap_y = y_pixels[y_mapped];
				int bitmap_x_low = bitmap_x >> info->level_ascending;
				int sample_x_low = bitmap_x_low % kBitmapWidth;
				int bitmap_y_low = bitmap_y >> info->level_ascending;
				int sample_y_low = bitmap_y_low % kBitmapHeight;
				src_index = sample_y_low * kBitmapWidth + sample_x_low;
			}
			src_index *= screen_bpp;

			// Formula for pixel offset is pixel[(y * width + x) * bpp]
			memcpy(dst + dst_index, src + src_index, screen_bpp);

			dst_index += screen_bpp;
		} // for x
	} // for y
}
void saim_rasterizer__initialize_bitmap_buffer(saim_rasterizer * rasterizer, 
	int key_min_x, int key_max_x, int key_min_y, int key_max_y, int level_of_detail, bool is_break)
{
	int width, height, bitmap_height;
	saim_bitmap_buffer * buffer = &rasterizer->bitmap_buffer;

	width = key_max_x - key_min_x + 1;
	if (width <= 0)
    	width += (1 << level_of_detail);
	height = key_max_y - key_min_y + 1;

	if (buffer->data) // there was some data
	{
		// Check whether we need to realloc data
		bitmap_height = buffer->key_max_y - buffer->key_min_y + 1;
		if (width > buffer->width || height > bitmap_height)
		{
			SAIM_FREE(buffer->data);
			buffer->data = (saim_buffered_bitmap_info *)SAIM_MALLOC(width * height * sizeof(saim_buffered_bitmap_info));
			memset(buffer->data, 0, width * height * sizeof(saim_buffered_bitmap_info));
		}
		else
		{
			// Otherwise we will just update the data
			memset(buffer->data, 0, buffer->width * bitmap_height * sizeof(saim_buffered_bitmap_info));
		}
	}
	else
	{
		buffer->data = (saim_buffered_bitmap_info *)SAIM_MALLOC(width * height * sizeof(saim_buffered_bitmap_info));
	}

	// Set new parameters
	buffer->key_min_x = key_min_x;
	buffer->width = width;
	buffer->key_min_y = key_min_y;
	buffer->key_max_x = key_max_x;
	buffer->key_max_y = key_max_y;
	buffer->level_of_detail = level_of_detail;
	buffer->is_break = is_break;
	buffer->has_any_low_detailed_bitmap = false;
}
void saim_rasterizer__set_bitmap_for_buffer(saim_rasterizer * rasterizer, int key_x, int key_y, const saim_bitmap * bitmap)
{
	int x, y, index;
	saim_buffered_bitmap_info * data;
	saim_bitmap_buffer * buffer = &rasterizer->bitmap_buffer;

	x = key_x - buffer->key_min_x;
	if (x < 0) // within break
	{
	    x += (1 << buffer->level_of_detail);
	}
	y = key_y - buffer->key_min_y;
	index = y * buffer->width + x;
#ifdef DEBUG
	assert(x >= 0 && x < buffer->width);
	assert(y >= 0 && y < buffer->key_max_y - buffer->key_min_y + 1);
#endif
	data = buffer->data + index;
	data->bitmap = bitmap;
	data->level_ascending = 0;
}
void saim_rasterizer__set_bitmap_low_for_buffer(saim_rasterizer * rasterizer, int key_z_low, int key_x, int key_y, const saim_bitmap * bitmap)
{
	int x, y, index;
	saim_buffered_bitmap_info * data;
	saim_bitmap_buffer * buffer = &rasterizer->bitmap_buffer;

	x = key_x - buffer->key_min_x;
	if (x < 0) // within break
	{
		x += (1 << buffer->level_of_detail);
	}
	y = key_y - buffer->key_min_y;
	index = y * buffer->width + x;
#ifdef DEBUG
	assert(x >= 0 && x < buffer->width);
	assert(y >= 0 && y < buffer->key_max_y - buffer->key_min_y + 1);
#endif
	data = buffer->data + index;
	data->bitmap = bitmap;
	data->level_ascending = buffer->level_of_detail - key_z_low;

	buffer->has_any_low_detailed_bitmap = true;
}
void saim_rasterizer__clear_bitmap_buffer(saim_rasterizer * rasterizer)
{
	saim_bitmap_buffer__destroy(&rasterizer->bitmap_buffer);
}
const saim_buffered_bitmap_info * saim_rasterizer__get_bitmap_from_buffer(saim_rasterizer * rasterizer, int key_x, int key_y)
{
	int x, y, index;
	saim_bitmap_buffer * buffer = &rasterizer->bitmap_buffer;

	bool in_x_range = (buffer->is_break)
		? (key_x >= buffer->key_min_x || key_x <= buffer->key_max_x)
		: (key_x >= buffer->key_min_x && key_x <= buffer->key_max_x);
	if (in_x_range && key_y >= buffer->key_min_y && key_y <= buffer->key_max_y)
	{
		x = key_x - buffer->key_min_x;
		if (x < 0) // within break
			x += (1 << buffer->level_of_detail);
		y = key_y - buffer->key_min_y;
		index = y * buffer->width + x;
		return buffer->data + index;
	}
	else
		return NULL;
}
void saim_rasterizer__erase_key_from_buffer(saim_rasterizer * rasterizer, const saim_data_key * key)
{
	int key_x, key_y, key_z;
	saim_buffered_bitmap_info * data;
	saim_bitmap_buffer * buffer = &rasterizer->bitmap_buffer;

	key_x = saim_data_key__get_x(key);
	key_y = saim_data_key__get_y(key);
	key_z = saim_data_key__get_z(key);
	if (buffer->has_any_low_detailed_bitmap && key_z != buffer->level_of_detail)
	{
		int tiles_per_side = 1 << buffer->level_of_detail;
		int height = buffer->key_max_y - buffer->key_min_y + 1;
		int index = 0;
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < buffer->width; ++x)
			{
				data = buffer->data + index;
				if (data->level_ascending != 0) // low detailed image
				{
					int cur_key_x = (buffer->key_min_x + x) % tiles_per_side;
					int cur_key_y = buffer->key_min_y + y;
					bool z_condition = (key_z + data->level_ascending == buffer->level_of_detail);
					bool x_condition = (key_x == (cur_key_x >> data->level_ascending));
					bool y_condition = (key_y == (cur_key_y >> data->level_ascending));
					if (z_condition && x_condition && y_condition)
					{
						data->bitmap = NULL;
						data->level_ascending = 0;
					}
				}
				++index;
			}
		}
	}
	else
	{
		bool in_x_range = (buffer->is_break)
			? (key_x >= buffer->key_min_x || key_x <= buffer->key_max_x)
			: (key_x >= buffer->key_min_x && key_x <= buffer->key_max_x);
		if (key_z == buffer->level_of_detail &&
			in_x_range &&
			key_y >= buffer->key_min_y && key_y <= buffer->key_max_y)
		{
			int x = key_x - buffer->key_min_x;
			if (x < 0) // within break
				x += (1 << buffer->level_of_detail);
			int y = key_y - buffer->key_min_y;
			int index = y * buffer->width + x;
			data = buffer->data + index;
			data->bitmap = NULL;
			data->level_ascending = 0;
		}
	}
}
void saim_rasterizer__sort_cache(saim_rasterizer * rasterizer)
{
	// Predicate is inside this function
	saim_bitmap_cache_info_list__sort(&rasterizer->bitmap_cache);
}

// --------------------------------------------------------------------------