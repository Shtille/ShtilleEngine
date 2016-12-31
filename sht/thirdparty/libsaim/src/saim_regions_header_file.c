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

#include "saim_regions_header_file.h"

#include "util/saim_memory.h"

#include <assert.h>

static const unsigned int kFormatSignature = 1213353295;

/*
File version changes:
Version 1.
Original version.
*/
static const unsigned char kVersion = 1;

// static const file_offset_t kHeaderSize =
// 	sizeof(unsigned int)  // signature
//   + sizeof(unsigned char) // version
//   + sizeof(unsigned char) // corruption
//   + sizeof(unsigned int)  // unique ID
//   + sizeof(unsigned int); // number of regions
static const file_offset_t kCorruptionByteOffset = sizeof(unsigned int) + sizeof(unsigned char);

void saim_regions_header_file__create(saim_regions_header_file * file, const char* filename)
{
	saim_file__create(&file->file, filename);
	file->unique_id = 0U;
	file->num_regions = 0U;
}
void saim_regions_header_file__destroy(saim_regions_header_file * file)
{
	saim_file__destroy(&file->file);
}
bool saim_regions_header_file__add_region(saim_regions_header_file * file, saim_region_map * map)
{
	++file->unique_id; // we need different ID for a new generated name
	++file->num_regions;
	return saim_regions_header_file__update(file, map);
}
bool saim_regions_header_file__remove_region(saim_regions_header_file * file, saim_region_map * map)
{
	--file->num_regions;
	return saim_regions_header_file__update(file, map);
}
bool saim_regions_header_file__update(saim_regions_header_file * file, saim_region_map * map)
{
	saim_set_node * node;
	saim_region_map_pair * pair;

	saim_file__close(&file->file);
	if (saim_file__open_for_write(&file->file))
	{
		saim_regions_header_file__write_header(file);
		// We will protect all write operations to avoid data corruption
		saim_regions_header_file__mark_operations_begin(file);

		// Enumerate map and write information
		node = saim_set_get_first(map->set);
		while (node != map->set->nil)
		{
			pair = (saim_region_map_pair *)node->data;
			saim_regions_header_file__write_region_stored_info(file, &pair->info);
			node = saim_set_get_next(map->set, node);
		}

		saim_regions_header_file__mark_operations_end(file);
		// Finally
		saim_file__close(&file->file);

		return file->file.operation_successful;
	}
	else
		return false;
}
void saim_regions_header_file__read_all_regions(saim_regions_header_file * file, saim_region_map * map)
{
	saim_region_map_pair * pair;
	for (unsigned int i = 0; i < file->num_regions; ++i)
	{
		pair = (saim_region_map_pair *)SAIM_MALLOC(sizeof(saim_region_map_pair));
		saim_region_map_pair__create(pair);
		saim_regions_header_file__read_region_stored_info(file, &pair->info);
		saim_string_copy(&pair->name, &pair->info.info.name);
		// Insert a pair into map
		(void)saim_region_map__insert(map, pair);
	}
}
bool saim_regions_header_file__flush(saim_regions_header_file * file)
{
	if (saim_file__open_for_write(&file->file))
	{
		file->unique_id = 0U;
		file->num_regions = 0U;
		file->file.operation_successful = true;
		saim_regions_header_file__write_header(file);
		saim_file__close(&file->file);
		return file->file.operation_successful;
	}
	else
		return false;
}
void saim_regions_header_file__regenerate(saim_regions_header_file * file)
{
	saim_file__close(&file->file);
	if (saim_file__open_for_write(&file->file))
	{
		saim_regions_header_file__write_header(file);
	}
	else
	{
		fprintf(stderr, "saim: file regeneration has failed.\n");
		file->file.operation_successful = false;
	}
}
void saim_regions_header_file__mark_operations_begin(saim_regions_header_file * file)
{
	file_offset_t position = saim_file__tell(&file->file);
	saim_file__offset_from_beginning(&file->file, kCorruptionByteOffset);
	saim_file__write_byte(&file->file, 1);
	saim_file__offset_from_beginning(&file->file, position);
}
void saim_regions_header_file__mark_operations_end(saim_regions_header_file * file)
{
	file_offset_t position = saim_file__tell(&file->file);
	saim_file__offset_from_beginning(&file->file, kCorruptionByteOffset);
	saim_file__write_byte(&file->file, 0);
	saim_file__offset_from_beginning(&file->file, position);
}
void saim_regions_header_file__write_header(saim_regions_header_file * file)
{
	saim_file__write_uint(&file->file, kFormatSignature);
	saim_file__write_byte(&file->file, kVersion);
	saim_file__write_byte(&file->file, 0); // corruption byte
	saim_file__write_uint(&file->file, file->unique_id);
	saim_file__write_uint(&file->file, file->num_regions);
}
void saim_regions_header_file__write_region_info(saim_regions_header_file * file, const saim_region_info * region)
{
	if (0 == fwrite(&region->upper_latitude, sizeof(region->upper_latitude), 1, file->file.file))
	{
		fprintf(stderr, "saim: fwrite failed\n");
		file->file.operation_successful = false;
	}
	if (0 == fwrite(&region->left_longitude, sizeof(region->left_longitude), 1, file->file.file))
	{
		fprintf(stderr, "saim: fwrite failed\n");
		file->file.operation_successful = false;
	}
	if (0 == fwrite(&region->lower_latitude, sizeof(region->lower_latitude), 1, file->file.file))
	{
		fprintf(stderr, "saim: fwrite failed\n");
		file->file.operation_successful = false;
	}
	if (0 == fwrite(&region->right_longitude, sizeof(region->right_longitude), 1, file->file.file))
	{
		fprintf(stderr, "saim: fwrite failed\n");
		file->file.operation_successful = false;
	}
	saim_file__write_string(&file->file, &region->name);
}
void saim_regions_header_file__write_region_stored_info(saim_regions_header_file * file, const saim_stored_region_info * region)
{
	saim_regions_header_file__write_region_info(file, &region->info);
	saim_file__write_uint(&file->file, region->status);
	if (0 == fwrite(&region->time, sizeof(region->time), 1, file->file.file))
	{
		fprintf(stderr, "saim: fwrite failed\n");
		file->file.operation_successful = false;
	}
	saim_file__write_string(&file->file, &region->filename);
}
bool saim_regions_header_file__read_header(saim_regions_header_file * file)
{
	unsigned int signature;
	unsigned char version;
	unsigned char corrupted;
	// Read signature
	saim_file__read_uint(&file->file, &signature);
	// Check signature
	if (signature != kFormatSignature)
	{
		fprintf(stderr, "saim: wrong file signature, terminating\n");
		file->file.operation_successful = false;
		return false;
	}
	// Read version
	saim_file__read_byte(&file->file, &version);
	// Check version
	if (version < kVersion) // out of date
	{
		fprintf(stderr, "saim: format version %d is out of date. File will be cleaned.\n", version);
		saim_regions_header_file__regenerate(file);
		return false;
	}
	// Read corruption byte
	saim_file__read_byte(&file->file, &corrupted);
	// Check corruption
	if (corrupted)
	{
		fprintf(stderr, "saim: file is broken. It will be cleaned.\n");
		saim_regions_header_file__regenerate(file);
		return false;
	}
	// Read unique id
	saim_file__read_uint(&file->file, &file->unique_id);
	// Read number of regions
	saim_file__read_uint(&file->file, &file->num_regions);
	return true;
}
void saim_regions_header_file__read_region_info(saim_regions_header_file * file, saim_region_info * region)
{
	if (0 == fread(&region->upper_latitude, sizeof(region->upper_latitude), 1, file->file.file))
	{
		fprintf(stderr, "saim: fread failed\n");
		file->file.operation_successful = false;
	}
	if (0 == fread(&region->left_longitude, sizeof(region->left_longitude), 1, file->file.file))
	{
		fprintf(stderr, "saim: fread failed\n");
		file->file.operation_successful = false;
	}
	if (0 == fread(&region->lower_latitude, sizeof(region->lower_latitude), 1, file->file.file))
	{
		fprintf(stderr, "saim: fread failed\n");
		file->file.operation_successful = false;
	}
	if (0 == fread(&region->right_longitude, sizeof(region->right_longitude), 1, file->file.file))
	{
		fprintf(stderr, "saim: fread failed\n");
		file->file.operation_successful = false;
	}
	saim_file__read_string(&file->file, &region->name);
}
void saim_regions_header_file__read_region_stored_info(saim_regions_header_file * file, saim_stored_region_info * region)
{
	saim_regions_header_file__read_region_info(file, &region->info);
	saim_file__read_uint(&file->file, &region->status);
	if (0 == fread(&region->time, sizeof(region->time), 1, file->file.file))
	{
		fprintf(stderr, "saim: fread failed\n");
		file->file.operation_successful = false;
	}
	saim_file__read_string(&file->file, &region->filename);
}