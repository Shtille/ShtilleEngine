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

#ifndef __SAIM_CACHE_H__
#define __SAIM_CACHE_H__

#include "saim_storage.h"
#include "saim_tile_service.h"
#include "saim_region_service.h"

typedef struct {
	saim_tile_service tile_service;
	saim_region_service region_service;
	saim_storage storage;
} saim_cache;

bool saim_cache__create(saim_cache * cache, const char* hash_string);
void saim_cache__destroy(saim_cache * cache);

bool saim_cache__initialize(saim_cache * cache);

//! Whether key exists in our database
bool saim_cache__is_exist(saim_cache * cache, const saim_data_key * key);

//! Query to load a single tile using tile service
void saim_cache__tile_service_load_query(saim_cache * cache, const saim_data_key * key, saim_tile_notification_function function);

//! Query to load a single tile using region service
void saim_cache__region_service_load_query(saim_cache * cache, const saim_data_key * key, const saim_string* name, saim_region_notification_function function);

//! Clear all regions download tasks
void saim_cache__clear_region_service_tasks(saim_cache * cache);

//! Should be called before any saim_cache__region_service_load_query call
bool saim_cache__region_add(saim_cache * cache, const saim_region_info * region_info);

bool saim_cache__region_rename(saim_cache * cache, const saim_string* old_name, const saim_string* new_name);

bool saim_cache__region_delete(saim_cache * cache, const saim_string* name);

bool saim_cache__region_mark_stored(saim_cache * cache, const saim_string* name);

bool saim_cache__region_mark_invalid(saim_cache * cache, const saim_string* name);

bool saim_cache__get_region_info(saim_cache * cache, const saim_string* name, saim_stored_region_info ** info);

big_file_size_t saim_cache__get_region_file_size(saim_cache * cache, const saim_string* name);

unsigned int saim_cache__get_key_count(saim_cache * cache);

//! Estimated storage file size for specified number of tiles and average data size
big_file_size_t saim_cache__get_estimated_storage_size(unsigned int num_tiles, unsigned int data_size);

#endif