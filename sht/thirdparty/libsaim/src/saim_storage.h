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

#ifndef __SAIM_STORAGE_H__
#define __SAIM_STORAGE_H__

#include "saim_key_pair.h"
#include "saim_region_info.h"
#include "saim_regions_header_file.h"
#include "saim_region_map.h"
#include "saim_storage_info_map.h"
#include "saim_key_set.h"

#include "../deps/tinycthread.h"

typedef enum {
	kStorage_Success,
	kStorage_Failed,
	kStorage_Regenerated
} initialize_result_t;

typedef struct saim_storage saim_storage;

struct saim_storage {
	unsigned int hash_value;						//!< stored hash value for selected provider
	mtx_t critical_section;							//!< critical section handle
	mtx_t critical_section_key_set;					//!< critical section handle for key set
	saim_key_set key_set;							//!< set of keys, done for fast rendering access
	saim_storage_info main_info;					//!< tile service data main file
	saim_regions_header_file regions_header_file;	//!< header file for regions
	saim_region_map region_map;						//!< map with regions information
	saim_storage_info_map saim_region_info__map;	//!< map with regions data information
};

/* =========== External functions =========== */

bool saim_storage__create(saim_storage * storage, const char* hash_string);
void saim_storage__destroy(saim_storage * storage);

//! Storage initialization
bool saim_storage__initialize(saim_storage * storage);

//! Called by tile service, finds storage for a key
key_pair_t * saim_storage__get_key_pair(saim_storage * storage, const saim_data_key * key, saim_storage_info ** storage_info);

//! Called by rendering thread, checks whether key is in any storage
bool saim_storage__is_exist(saim_storage * storage, const saim_data_key * key);

//! Called by tile service, loads data from a chosen source
bool saim_storage__load(saim_storage * storage, key_pair_t * pair, saim_string * data, saim_storage_info * info);

//! Called by tile service, saves data to main storage file
bool saim_storage__save_main(saim_storage * storage, const saim_data_key * key, const saim_string * data);

//! Called by region service, saves data to some named region storage file
bool saim_storage__save_separate(saim_storage * storage, const saim_data_key * key, const saim_string * data, const saim_string * name);

bool saim_storage__region_add(saim_storage * storage, const saim_region_info * region_info);
bool saim_storage__region_rename(saim_storage * storage, const saim_string * old_name, const saim_string * new_name);
bool saim_storage__region_delete(saim_storage * storage, const saim_string * name);
bool saim_storage__region_mark_stored(saim_storage * storage, const saim_string * name);
bool saim_storage__region_mark_invalid(saim_storage * storage, const saim_string * name);

//! Returns information about region
bool saim_storage__get_region_info(saim_storage * storage, const saim_string * name, saim_stored_region_info ** info);
//! Returns information about regions
//bool storage_get_regions_info(saim_storage * storage, std::vector<StoredRegionInfo>& regions_info);
//! Returns region file size in bytes
big_file_size_t saim_storage__get_region_file_size(saim_storage * storage, const saim_string * name);

unsigned int saim_storage__get_key_count(saim_storage * storage);

//! Estimated storage file size for specified number of tiles and average data size
big_file_size_t saim_storage__get_estimated_file_size(unsigned int num_tiles, unsigned int data_size);

//! Maximum storage file size for a region
big_file_size_t saim_storage__get_maximum_region_file_size();

/* =========== Internal functions =========== */

initialize_result_t saim_storage__initialize_file(saim_storage * storage, saim_storage_info * info);
bool saim_storage__initialize_regions_header_file(saim_storage * storage);
void saim_storage__initialize_key_set(saim_storage * storage);
void saim_storage__deinitialize_regions(saim_storage * storage);
void saim_storage__deinitialize(saim_storage * storage);
void saim_storage__generate_region_file_name(saim_storage * storage, char* buffer);
bool saim_storage__save(saim_storage * storage, const saim_data_key * key, const saim_string * data, saim_storage_info * info);

#endif