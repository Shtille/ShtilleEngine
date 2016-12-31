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

#include "saim_storage.h"

#include "util/saim_memory.h"
#include "util/saim_file_op.h"

#include <string.h>

static const file_offset_t kMaxMainFileSize = 300 << 20; // 300 MB
static const file_offset_t kMaxRegionFileSize = 500 << 20; // 500 MB

extern char s_path[260]; // declared in saim_init.c

static void append_path(char * buffer, const char* filename)
{
	size_t path_length, filename_length;
	path_length = strlen(s_path);
	filename_length = strlen(filename);
	strncpy(buffer, s_path, path_length);
	strncpy(buffer + path_length, filename, filename_length);
	buffer[path_length + filename_length] = '\0';
}

bool saim_storage__create(saim_storage * storage, const char* hash_string)
{
	if (mtx_init(&storage->critical_section, mtx_plain) == thrd_error)
	{
		fprintf(stderr, "saim: mutex init failed\n");
		return false;
	}
	if (mtx_init(&storage->critical_section_key_set, mtx_plain) == thrd_error)
	{
		fprintf(stderr, "saim: mutex init failed\n");
		return false;
	}
	storage->hash_value = saim_hash(hash_string);
	saim_key_set__create(&storage->key_set);
	saim_region_map__create(&storage->region_map);
	saim_storage_info_map__create(&storage->saim_region_info__map);
	return true;
}
void saim_storage__destroy(saim_storage * storage)
{
	saim_storage__deinitialize(storage);
	saim_storage_info_map__destroy(&storage->saim_region_info__map);
	saim_region_map__destroy(&storage->region_map);
	saim_key_set__destroy(&storage->key_set);
	mtx_destroy(&storage->critical_section_key_set);
	mtx_destroy(&storage->critical_section);
}
bool saim_storage__initialize(saim_storage * storage)
{
	char buffer[260];
	saim_set_node *node, *nil;
	saim_region_map_pair * pair;
	saim_storage_info_pair * info_pair;
	initialize_result_t result;

	append_path(buffer, "regions.omrh");
	saim_regions_header_file__create(&storage->regions_header_file, buffer);
	append_path(buffer, "data.om");
	saim_storage_info__create(&storage->main_info);
	saim_storage_file__create(&storage->main_info.file, buffer, storage->hash_value, kMaxMainFileSize);
	if (saim_storage__initialize_file(storage, &storage->main_info) == kStorage_Failed)
		return false;
	if (!saim_storage__initialize_regions_header_file(storage))
		return false;
	// Initialize region files
	nil = storage->region_map.set->nil;
	node = saim_set_get_first(storage->region_map.set);
	while (node != nil)
	{
		pair = (saim_region_map_pair *)node->data;
		info_pair = SAIM_MALLOC(sizeof(saim_storage_info_pair));
		saim_storage_info_pair__create(info_pair);
		saim_string_set(&info_pair->name, &pair->name);
		saim_storage_file__create(&info_pair->info.file, pair->info.filename.data, storage->hash_value, kMaxRegionFileSize);
		saim_storage_info_map__insert(&storage->saim_region_info__map, info_pair);
		result = saim_storage__initialize_file(storage, &info_pair->info);
		if (result == kStorage_Failed) // offsets & list
			return false;
		if (result == kStorage_Regenerated) // region storage file has been corrupted
		{
			fprintf(stderr, "saim: region '%s' storage file has been broken\n", pair->name.data);
			pair->info.status = kRegion_Invalid;
			// Update header file information
			if (!saim_regions_header_file__update(&storage->regions_header_file, &storage->region_map))
				return false;
		}
		if (pair->info.status == kRegion_Invalid)
		{
			fprintf(stderr, "saim: region '%s' is invalid\n", pair->name.data);
		}
		node = saim_set_get_next(storage->region_map.set, node);
	}
	saim_storage__initialize_key_set(storage);
	return true;
}
key_pair_t * saim_storage__get_key_pair(saim_storage * storage, const saim_data_key * key, saim_storage_info ** storage_info)
{
	saim_set_node *node, *nil, *node2, *nil2;
	saim_storage_info_pair * info_pair;
	key_pair_t * return_pair = 0;

	mtx_lock(&storage->critical_section);
	// First time search in the main storage file
	nil = storage->main_info.offsets.set->nil;
	node = saim_key_offset_map__search(&storage->main_info.offsets, key);
	if (node != nil)
	{
		*storage_info = &storage->main_info; // specify main data source
		return_pair = (key_pair_t *)node->data;
	}
	else // Try to find this key in regions
	{
		nil = storage->saim_region_info__map.set->nil;
    	node = saim_set_get_first(storage->saim_region_info__map.set);
    	while (node != nil)
    	{
    		info_pair = (saim_storage_info_pair *)node->data;
    		nil2 = info_pair->info.offsets.set->nil;
    		node2 = saim_key_offset_map__search(&info_pair->info.offsets, key);
    		if (node2 != nil2)
    		{
    			*storage_info = &info_pair->info; // specify region data source
    			return_pair = (key_pair_t *)node2->data;
    			break;
    		}
    		node = saim_set_get_next(storage->saim_region_info__map.set, node);
    	}
	}
	mtx_unlock(&storage->critical_section);
	return return_pair;
}
bool saim_storage__is_exist(saim_storage * storage, const saim_data_key * key)
{
	saim_set_node * node;
	bool is_exist;

	mtx_lock(&storage->critical_section_key_set);
	node = saim_set_search(storage->key_set.set, key);
	is_exist = (node != storage->key_set.set->nil);
	mtx_unlock(&storage->critical_section_key_set);
	return is_exist;
}
bool saim_storage__load(saim_storage * storage, key_pair_t * pair, saim_string * data, saim_storage_info * info)
{
	saim_file * base_file = &info->file.file;
	stored_key_pair_t stored_pair;
	bool result = false;

	mtx_lock(&storage->critical_section);
	if (saim_file__open_for_read(base_file))
	{
		// Increase counter of found element in the map
		++pair->info.counter;
		// List contains iterators to map, so we just should sort it
		saim_map_nodes_list__sort(&info->list);
		// We should update counter value in the file too
		make_stored_key_pair(&stored_pair, pair);
		saim_file__offset_from_beginning(base_file, pair->info.key_offset);
		saim_storage_file__write_key_pair(&info->file, &stored_pair);
		// Offset to data
		saim_file__offset_from_beginning(base_file, pair->info.data_offset);
		// Allocate space for data
		saim_string_resize(data, pair->info.size);
		// Read the data
		saim_file__read_array(base_file, data->data, pair->info.size);
		// Finally
		saim_file__close(base_file);
		result = true;
	}
	mtx_unlock(&storage->critical_section);
	return result;
}
bool saim_storage__save_main(saim_storage * storage, const saim_data_key * key, const saim_string * data)
{
	bool result;
	mtx_lock(&storage->critical_section);
	result = saim_storage__save(storage, key, data, &storage->main_info);
	mtx_unlock(&storage->critical_section);
	return result;
}
bool saim_storage__save_separate(saim_storage * storage, const saim_data_key * key, const saim_string * data, const saim_string * name)
{
	bool result;
	saim_set_node * node;
	saim_storage_info_pair * pair;

	mtx_lock(&storage->critical_section);
	node = saim_storage_info_map__search(&storage->saim_region_info__map, name);
	if (node == storage->saim_region_info__map.set->nil)
	{
		fprintf(stderr, "saim: did not find region storage with name '%s'\n", name->data);
		result = false;
	}
	else // normal case
	{
		pair = (saim_storage_info_pair *)node->data;
		result = saim_storage__save(storage, key, data, &pair->info);
	}
	mtx_unlock(&storage->critical_section);
	return result;
}
bool saim_storage__region_add(saim_storage * storage, const saim_region_info * region_info)
{
	char buffer[260];
	saim_set_node * node;
	saim_storage_info_pair * info_pair;
	saim_region_map_pair * region_pair;

	mtx_lock(&storage->critical_section);

	node = saim_region_map__search(&storage->region_map, &region_info->name);
	if (node != storage->region_map.set->nil)
	{
		fprintf(stderr, "saim: region '%s' already exists\n", region_info->name.data);
		mtx_unlock(&storage->critical_section);
        return false; // already exists
	}
	// Generate a new filename
	saim_storage__generate_region_file_name(storage, buffer);

	// Add a new object to map
    region_pair = (saim_region_map_pair *)SAIM_MALLOC(sizeof(saim_region_map_pair));
    saim_region_map_pair__create(region_pair);
    saim_string_set(&region_pair->name, &region_info->name);
    (void)saim_region_map__insert(&storage->region_map, region_pair);
    saim_region_info__copy(&region_pair->info.info, region_info);
    region_pair->info.status = kRegion_Downloading;
    region_pair->info.time = time(NULL); // use current time
    saim_string_set_text(&region_pair->info.filename, buffer);

	// Add file to region info map
	info_pair = (saim_storage_info_pair *)SAIM_MALLOC(sizeof(saim_storage_info_pair));
	saim_storage_info_pair__create(info_pair);
	saim_string_set(&info_pair->name, &region_info->name);
	(void)saim_storage_info_map__insert(&storage->saim_region_info__map, info_pair);

	// Create a new storage file
	saim_storage_file__create(&info_pair->info.file, buffer, storage->hash_value, kMaxRegionFileSize);

	// Create file on disk
	saim_storage_file__regenerate(&info_pair->info.file);

	// Write information into header file
    if (!saim_regions_header_file__add_region(&storage->regions_header_file, &storage->region_map))
    {
    	mtx_unlock(&storage->critical_section);
        return false;
    }

	mtx_unlock(&storage->critical_section);

	return true;
}
bool saim_storage__region_rename(saim_storage * storage, const saim_string * old_name, const saim_string * new_name)
{
	saim_set_node * node;
	saim_region_map_pair *region_pair;
	saim_storage_info_pair * info_pair;

	mtx_lock(&storage->critical_section);

	node = saim_region_map__search(&storage->region_map, old_name);
	if (node == storage->region_map.set->nil)
	{
		fprintf(stderr, "saim: region '%s' hasn't been found\n", old_name->data);
		mtx_unlock(&storage->critical_section);
        return false; // didn't find old one
	}

	// Change map object (name is also a map key, so we have to erase element)
	region_pair = (saim_region_map_pair *)node->data;
	saim_region_map__cut(&storage->region_map, node);
	saim_string_set(&region_pair->name, new_name);
	saim_string_set(&region_pair->info.info.name, new_name);
	(void)saim_region_map__insert(&storage->region_map, region_pair);

	// Also need to change files map
	node = saim_storage_info_map__search(&storage->saim_region_info__map, old_name);
	info_pair = (saim_storage_info_pair *)node->data;
	saim_storage_info_map__cut(&storage->saim_region_info__map, node);
	saim_string_set(&info_pair->name, new_name);
	(void)saim_storage_info_map__insert(&storage->saim_region_info__map, info_pair);

	// Update header file information
	if (!saim_regions_header_file__update(&storage->regions_header_file, &storage->region_map))
	{
		mtx_unlock(&storage->critical_section);
		return false;
	}

	mtx_unlock(&storage->critical_section);

	return true;
}
bool saim_storage__region_delete(saim_storage * storage, const saim_string * name)
{
	saim_set_node * node;
	saim_storage_info_pair * info_pair;

	mtx_lock(&storage->critical_section);

	node = saim_region_map__search(&storage->region_map, name);
	if (node == storage->region_map.set->nil)
	{
		fprintf(stderr, "saim: region '%s' hasn't been found\n", name->data);
		mtx_unlock(&storage->critical_section);
        return false; // didn't find old one
	}

	// Remove region from map
	saim_region_map__erase(&storage->region_map, node);

	// Remove region file from list
	node = saim_storage_info_map__search(&storage->saim_region_info__map, name);
	info_pair = (saim_storage_info_pair *)node->data;

	// Delete file from disk
	saim_file_op__delete(info_pair->info.file.file.filename);

	saim_storage_file__destroy(&info_pair->info.file);
	saim_storage_info_map__erase(&storage->saim_region_info__map, node);

	// Need to reinitialize key set
	mtx_lock(&storage->critical_section_key_set);
	saim_storage__initialize_key_set(storage);
	mtx_unlock(&storage->critical_section_key_set);

	// Update header file information
	if (!saim_regions_header_file__remove_region(&storage->regions_header_file, &storage->region_map))
	{
		mtx_unlock(&storage->critical_section);
		return false;
	}

	mtx_unlock(&storage->critical_section);

	return true;
}
bool saim_storage__region_mark_stored(saim_storage * storage, const saim_string * name)
{
	saim_set_node * node;
	saim_region_map_pair * region_pair;

	mtx_lock(&storage->critical_section);

	node = saim_region_map__search(&storage->region_map, name);
	if (node == storage->region_map.set->nil)
	{
		fprintf(stderr, "saim: region '%s' hasn't been found\n", name->data);
		mtx_unlock(&storage->critical_section);
        return false; // didn't find one
	}

	// Update map data
	region_pair = (saim_region_map_pair *)node->data;
	region_pair->info.status = kRegion_Stored;

	// Update header file information
	if (!saim_regions_header_file__update(&storage->regions_header_file, &storage->region_map))
	{
		mtx_unlock(&storage->critical_section);
		return false;
	}

	mtx_unlock(&storage->critical_section);

	return true;
}
bool saim_storage__region_mark_invalid(saim_storage * storage, const saim_string * name)
{
	saim_set_node * node;
	saim_region_map_pair * region_pair;

	mtx_lock(&storage->critical_section);

	node = saim_region_map__search(&storage->region_map, name);
	if (node == storage->region_map.set->nil)
	{
		fprintf(stderr, "saim: region '%s' hasn't been found\n", name->data);
		mtx_unlock(&storage->critical_section);
        return false; // didn't find one
	}

	// Update map data
	region_pair = (saim_region_map_pair *)node->data;
	region_pair->info.status = kRegion_Invalid;

	// Update header file information
	if (!saim_regions_header_file__update(&storage->regions_header_file, &storage->region_map))
	{
		mtx_unlock(&storage->critical_section);
		return false;
	}

	mtx_unlock(&storage->critical_section);

	return true;
}
bool saim_storage__get_region_info(saim_storage * storage, const saim_string * name, saim_stored_region_info ** info)
{
	bool result;
	saim_set_node * node;
	saim_region_map_pair * pair;

	mtx_lock(&storage->critical_section);
	node = saim_region_map__search(&storage->region_map, name);
	if (node == storage->region_map.set->nil)
	{
		result = false;
	}
	else // normal case
	{
		pair = (saim_region_map_pair *)node->data;
		*info = &pair->info;
		result = true;
	}
	mtx_unlock(&storage->critical_section);
	return result;
}
big_file_size_t saim_storage__get_region_file_size(saim_storage * storage, const saim_string * name)
{
	return 0;
}
unsigned int saim_storage__get_key_count(saim_storage * storage)
{
	unsigned int count;
	mtx_lock(&storage->critical_section_key_set);
	count = saim_key_set__size(&storage->key_set);
	mtx_unlock(&storage->critical_section_key_set);
	return count;
}
big_file_size_t saim_storage__get_estimated_file_size(unsigned int num_tiles, unsigned int data_size)
{
	return saim_storage_file__get_estimated_file_size(num_tiles, data_size);
}
big_file_size_t saim_storage__get_maximum_region_file_size()
{
	return (big_file_size_t)kMaxRegionFileSize;
}
initialize_result_t saim_storage__initialize_file(saim_storage * storage, saim_storage_info * info)
{
	saim_file * base_file = &info->file.file;
	 // Read storage setting from header and initialize the key map
	if (saim_file__open_for_read(base_file))
	{
	    if (saim_storage_file__read_header(&info->file))
	    {
	        if (!base_file->operation_successful)
	        {
	        	saim_file__close(base_file);
	            return kStorage_Failed;
	        }
	        // Initialize key map
	        saim_storage_file__read_all_keys(&info->file, &info->offsets, &info->list);
	        saim_file__close(base_file);
	        return kStorage_Success;
	    }
	    else if (base_file->operation_successful)
	    {
	        // The file has been regenerated
	        saim_file__close(base_file);
	        return kStorage_Regenerated;
	    }
	    else
	    {
	        // File regeneration has been failed
	        saim_file__close(base_file);
	        return kStorage_Failed;
	    }
	}
	else if (saim_file__open_for_write(base_file))
	{
	    fprintf(stdout, "saim: storage initialization\n");
	    // Seems like file doesn't exist, let's create a new one
	    saim_storage_file__write_header(&info->file);
	    if (!base_file->operation_successful)
	    {
	        saim_file__close(base_file);
	        return kStorage_Failed;
	    }
	    // There are no data yet, so we won't fill our map.
	    saim_file__close(base_file);
	    return kStorage_Success;
	}
	else
	    return kStorage_Failed;
}
bool saim_storage__initialize_regions_header_file(saim_storage * storage)
{
	saim_file * base_file = &storage->regions_header_file.file;
	// Read storage setting from header and initialize the key map
    if (saim_file__open_for_read(base_file))
    {
        if (saim_regions_header_file__read_header(&storage->regions_header_file))
        {
            if (!base_file->operation_successful)
            {
                saim_file__close(base_file);
                return false;
            }
            // Initialize key map
            saim_regions_header_file__read_all_regions(&storage->regions_header_file, &storage->region_map);
            saim_file__close(base_file);
            return true;
        }
        else if (base_file->operation_successful)
        {
            // The file has been regenerated
            saim_file__close(base_file);
            return true;
        }
        else
        {
            // File regeneration has been failed
            saim_file__close(base_file);
            return false;
        }
    }
    else if (saim_file__open_for_write(base_file))
    {
        fprintf(stdout, "saim: regions header file initialization\n");
        // Seems like file doesn't exist, let's create a new one
        saim_regions_header_file__write_header(&storage->regions_header_file);
        if (!base_file->operation_successful)
        {
            saim_file__close(base_file);
            return false;
        }
        // There are no data yet, so we won't fill our map.
        saim_file__close(base_file);
        return true;
    }
    else
        return false;
}
void saim_storage__initialize_key_set(saim_storage * storage)
{
	saim_set_node *node, *node2;
	saim_set_node *nil, *nil2;
	key_pair_t * pair;
	saim_data_key * key_copy;
	saim_storage_info_pair * info_pair;

	saim_key_set__clear(&storage->key_set);
    // From main storage
    nil = storage->main_info.offsets.set->nil;
    node = saim_set_get_first(storage->main_info.offsets.set);
    while (node != nil)
    {
    	pair = (key_pair_t *)node->data;
		key_copy = (saim_data_key *)SAIM_MALLOC(sizeof(saim_data_key));
		saim_data_key__set_by_other(key_copy, &pair->key);
    	saim_key_set__insert(&storage->key_set, key_copy);
    	node = saim_set_get_next(storage->main_info.offsets.set, node);
    }
    // From regions storages
    nil = storage->saim_region_info__map.set->nil;
    node = saim_set_get_first(storage->saim_region_info__map.set);
    while (node != nil)
    {
    	info_pair = (saim_storage_info_pair *)node->data;

    	nil2 = info_pair->info.offsets.set->nil;
    	node2 = saim_set_get_first(info_pair->info.offsets.set);
    	while (node2 != nil2)
    	{
    		pair = (key_pair_t *)node2->data;
			key_copy = (saim_data_key *)SAIM_MALLOC(sizeof(saim_data_key));
			saim_data_key__set_by_other(key_copy, &pair->key);
    		saim_key_set__insert(&storage->key_set, key_copy);
    		node2 = saim_set_get_next(info_pair->info.offsets.set, node2);
    	}
    	node = saim_set_get_next(storage->saim_region_info__map.set, node);
    }
}
void saim_storage__deinitialize_regions(saim_storage * storage)
{
	saim_set_node *node, *nil;
	saim_storage_info_pair * info_pair;

	saim_region_map__clear(&storage->region_map);
	nil = storage->saim_region_info__map.set->nil;
    node = saim_set_get_first(storage->saim_region_info__map.set);
    while (node != nil)
    {
    	info_pair = (saim_storage_info_pair *)node->data;
    	saim_storage_file__destroy(&info_pair->info.file);
    	node = saim_set_get_next(storage->saim_region_info__map.set, node);
    }
    saim_storage_info_map__clear(&storage->saim_region_info__map);
}
void saim_storage__deinitialize(saim_storage * storage)
{
	saim_storage__deinitialize_regions(storage);
	saim_storage_file__destroy(&storage->main_info.file);
	saim_storage_info__destroy(&storage->main_info);
	saim_regions_header_file__destroy(&storage->regions_header_file);
}
void saim_storage__generate_region_file_name(saim_storage * storage, char* buffer)
{
	unsigned int id;
	char filename[30];

	id = storage->regions_header_file.unique_id;
	sprintf(filename, "%u.omr", id);
	append_path(buffer, filename);
}
bool saim_storage__save(saim_storage * storage, const saim_data_key * key, const saim_string * data, saim_storage_info * info)
{
	saim_file * base_file = &info->file.file;
	save_result_t result;
	saim_data_key * key_copy;
	bool need_to_flush;

	if (data->length == 0)
	{
		fprintf(stderr, "saim: curl returned empty data for key(%i,%i,%i)\n",
                      saim_data_key__get_z(key), saim_data_key__get_x(key), saim_data_key__get_y(key));
		return true;
	}
	if (saim_file__open_for_read(base_file))
	{
		// We will protect all write operations to avoid data corruption
		// (application may crash due to external reason, a.e.)
		saim_storage_file__mark_operations_begin(&info->file);
		// The main save logics is done here
		result = saim_storage_file__save_logics(&info->file, key, data, &info->offsets, &info->list);
		saim_storage_file__mark_operations_end(&info->file);
		// Finally
		saim_file__close(base_file);
		need_to_flush = false;
		if (result == kSave_NeedToFlush)
		{
			fprintf(stdout, "saim: storage flush due to reaching hard cap\n");
			if (saim_storage_file__flush(&info->file))
			{
				need_to_flush = true;
				saim_key_offset_map__clear(&info->offsets);
				saim_map_nodes_list__clear(&info->list);
			}
		}
		mtx_lock(&storage->critical_section_key_set);
		if (need_to_flush)
			saim_storage__initialize_key_set(storage);
		else
		{
			key_copy = (saim_data_key *)SAIM_MALLOC(sizeof(saim_data_key));
			saim_data_key__set_by_other(key_copy, key);
			saim_key_set__insert(&storage->key_set, key_copy);
		}
		mtx_unlock(&storage->critical_section_key_set);
		return result == kSave_Success;
	}
	else
		return false;
}