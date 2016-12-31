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

#include "saim_storage_file.h"

#include "util/saim_memory.h"

#include <assert.h>

static const unsigned int kFormatSignature = 1397116239;

/*
File version changes:
Version 1.
    Original version.
Version 2.
    Added corruption byte into the header.
Version 3.
    Added usage counter into key info.
Version 4.
    Added provider name hash value.
Version 5.
    Changed key hash format.
*/
static const unsigned char kVersion = 5;

static const file_offset_t kCorruptionByteOffset = sizeof(unsigned int) + sizeof(unsigned char);
static const file_offset_t kHeaderSize =
      sizeof(unsigned int)  // signature
    + sizeof(unsigned char) // version
    + sizeof(unsigned char) // corruption
    + sizeof(int)           // maximum file size
    + sizeof(unsigned int)  // provider name hash value
    + sizeof(unsigned int)  // block capacity
    + sizeof(unsigned int); // blocks count

static const file_offset_t kMaxFileSizeTolerance = 10 * (1 << 20);

void saim_storage_file__create(saim_storage_file * file,
	const char* filename, unsigned int hash_value, file_offset_t max_file_size)
{
	saim_file__create(&file->file, filename);
	file->max_file_size = max_file_size;
	file->hash_value = hash_value;
	file->block_capacity = 100;
	file->blocks_count = 0;
	file->need_to_shrink = false;
}
void saim_storage_file__destroy(saim_storage_file * file)
{
	saim_file__destroy(&file->file);
}
big_file_size_t saim_storage_file__get_estimated_file_size(unsigned int num_tiles, unsigned int data_size)
{
	unsigned int num_blocks = num_tiles / SAIM_FILE_BLOCK_CAPACITY;
	if (num_tiles % SAIM_FILE_BLOCK_CAPACITY != 0)
	    ++num_blocks;
	big_file_size_t block_size = (big_file_size_t)(sizeof(block_header_t) + sizeof(block_keys_t));
	big_file_size_t size = (big_file_size_t)(kHeaderSize);
	size += block_size * (big_file_size_t)(num_blocks);
	size += (big_file_size_t)(num_tiles) * (big_file_size_t)(data_size);
	return size;
}
bool saim_storage_file__flush(saim_storage_file * file)
{
	if (saim_file__open_for_write(&file->file))
    {
        file->file.operation_successful = true;
        file->blocks_count = 0;
        saim_storage_file__write_header(file);
        saim_file__close(&file->file);
        return file->file.operation_successful;
    }
    else
        return false;
}
void saim_storage_file__regenerate(saim_storage_file * file)
{
	saim_file__close(&file->file);
	if (saim_file__open_for_write(&file->file))
	{
		saim_storage_file__write_header(file);
	}
	else
	{
		fprintf(stderr, "saim: file regeneration has failed.\n");
		file->file.operation_successful = false;
	}
}
void saim_storage_file__mark_operations_begin(saim_storage_file * file)
{
	file_offset_t position = saim_file__tell(&file->file);
	saim_file__offset_from_beginning(&file->file, kCorruptionByteOffset);
	saim_file__write_byte(&file->file, 1);
	saim_file__offset_from_beginning(&file->file, position);
}
void saim_storage_file__mark_operations_end(saim_storage_file * file)
{
	file_offset_t position = saim_file__tell(&file->file);
	saim_file__offset_from_beginning(&file->file, kCorruptionByteOffset);
	saim_file__write_byte(&file->file, 0);
	saim_file__offset_from_beginning(&file->file, position);
}
void saim_storage_file__write_header(saim_storage_file * file)
{
	saim_file__write_uint(&file->file, kFormatSignature);
	saim_file__write_byte(&file->file, kVersion);
	saim_file__write_byte(&file->file, 0); // corruption byte
	saim_file__write_int(&file->file, file->max_file_size);
	saim_file__write_uint(&file->file, file->hash_value);
	saim_file__write_uint(&file->file, file->block_capacity);
	saim_file__write_uint(&file->file, file->blocks_count);
}
bool saim_storage_file__read_header(saim_storage_file * file)
{
	unsigned int signature;
	unsigned char version;
	unsigned char corrupted;
	unsigned int hash_value;
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
		saim_storage_file__regenerate(file);
		return false;
	}
	// Read corruption byte
	saim_file__read_byte(&file->file, &corrupted);
	// Check corruption
	if (corrupted)
	{
		fprintf(stderr, "saim: file is broken. It will be cleaned.\n");
		saim_storage_file__regenerate(file);
		return false;
	}
	// Read max file size
	saim_file__read_int(&file->file, &file->max_file_size);
	// Read provider hash value
	saim_file__read_uint(&file->file, &hash_value);
	if (hash_value != file->hash_value)
	{
		fprintf(stderr, "saim: file provider differs from chosen. File will be cleaned.\n");
		saim_storage_file__regenerate(file);
		return false;
	}
	// Read block capacity
	saim_file__read_uint(&file->file, &file->block_capacity);
	// Read blocks count
	saim_file__read_uint(&file->file, &file->blocks_count);
	return true;
}
void saim_storage_file__read_all_keys(saim_storage_file * file, saim_key_offset_map * offsets, saim_map_nodes_list * list)
{
	file_offset_t block_offset;
	block_header_t block_header;
	file_offset_t pair_offset;
	stored_key_pair_t stored_pair;
	key_pair_t * pair;
	saim_set_node * node;

	block_offset = saim_file__tell(&file->file);

	// Assumed that we've read the header
	for (unsigned int i = 0; i < file->blocks_count; ++i)
	{
	    // Seek to the block position
	    saim_file__offset_from_beginning(&file->file, block_offset);

	    // Read block header
	    saim_storage_file__read_block_header(file, &block_header);
	    block_offset = block_header.next_offset;

	    // Read keys
	    for (unsigned int i = 0; i < block_header.count; ++i)
	    {
	        pair_offset = saim_file__tell(&file->file);

	        saim_storage_file__read_key_pair(file, &stored_pair);

			// Convert stored key pair into key pair
			pair = (key_pair_t *)SAIM_MALLOC(sizeof(key_pair_t));
			make_key_pair(pair, &stored_pair, pair_offset);

			// Insert pair into map
			node = saim_key_offset_map__insert(offsets, pair);

			// Add pair pointer into list
			saim_map_nodes_list__insert(list, node);
	    }
	}
	saim_map_nodes_list__sort(list);
}
void saim_storage_file__write_key_pair(saim_storage_file * file, const stored_key_pair_t* pair)
{
	if (0 == fwrite(&(pair->key), sizeof(pair->key), 1, file->file.file))
	{
		fprintf(stderr, "saim: fwrite failed\n");
		file->file.operation_successful = false;
	}
	if (0 == fwrite(&(pair->info), sizeof(pair->info), 1, file->file.file))
	{
		fprintf(stderr, "saim: fwrite failed\n");
		file->file.operation_successful = false;
	}
}
void saim_storage_file__read_key_pair(saim_storage_file * file, stored_key_pair_t *pair)
{
	if (0 == fread(&(pair->key), sizeof(pair->key), 1, file->file.file))
	{
		fprintf(stderr, "saim: fread failed\n");
		file->file.operation_successful = false;
	}
	if (0 == fread(&(pair->info), sizeof(pair->info), 1, file->file.file))
	{
		fprintf(stderr, "saim: fread failed\n");
		file->file.operation_successful = false;
	}
}
void saim_storage_file__write_block_header(saim_storage_file * file, const block_header_t* block_header)
{
	saim_file__write_uint(&file->file, block_header->count);
	saim_file__write_int(&file->file, block_header->next_offset);
}
void saim_storage_file__read_block_header(saim_storage_file * file, block_header_t *block_header)
{
	saim_file__read_uint(&file->file, &(block_header->count));
	saim_file__read_int(&file->file, &(block_header->next_offset));
}
void saim_storage_file__write_block_keys(saim_storage_file * file, const block_keys_t* keys)
{
	if (0 == fwrite(keys->key_offsets, sizeof(keys->key_offsets), 1, file->file.file))
	{
		fprintf(stderr, "saim: fwrite failed\n");
		file->file.operation_successful = false;
	}
}
void saim_storage_file__read_block_keys(saim_storage_file * file, block_keys_t *keys)
{
	if (0 == fread(keys->key_offsets, sizeof(keys->key_offsets), 1, file->file.file))
	{
		fprintf(stderr, "saim: fread failed\n");
		file->file.operation_successful = false;
	}
}
save_result_t saim_storage_file__replace_key(saim_storage_file * file,
	const saim_data_key* key, const saim_string * data,
    saim_key_offset_map * offsets, saim_map_nodes_list * list)
{
	file_size_t data_size;
	saim_list_node * found_list_node;
	saim_set_node * found_node;
	key_pair_t * found_pair;
	key_pair_t * pair;
	stored_key_pair_t stored_pair;

	data_size = (file_size_t)data->length;

	// At first we should determine which key is to replace
	found_list_node = saim_map_nodes_list__find_size_compatible(list, data_size);
	// Replace existable key struct
	pair = (key_pair_t *)SAIM_MALLOC(sizeof(key_pair_t));
	pair->key = *key;
	pair->info.size = data_size;
	pair->info.counter = 0U;
	if (found_list_node != NULL) // found a replace
	{
		found_node = (saim_set_node *)found_list_node->data;
		found_pair = (key_pair_t *)found_node->data;
		saim_file__offset_from_beginning(&file->file, found_pair->info.data_offset);
		pair->info.key_offset = found_pair->info.key_offset;
		pair->info.data_offset = found_pair->info.data_offset;
	}
	else // !found_replace
	{
		// Replace first found pair
		found_list_node = saim_map_nodes_list__get_first(list);
		found_node = (saim_set_node *)found_list_node->data;
		found_pair = (key_pair_t *)found_node->data;
		pair->info.key_offset = found_pair->info.key_offset;
		// Old data hasn't enough space for our new data
		// So we gonna write data at the end of the file
		saim_file__offset_from_end(&file->file, 0);
		pair->info.data_offset = saim_file__tell(&file->file);
		if (pair->info.data_offset + (file_offset_t)data_size > 
			file->max_file_size + kMaxFileSizeTolerance)
		{
			// We have reached hard cap, so flush the file
			SAIM_FREE(pair);
			return kSave_NeedToFlush;
		}
	}
	saim_file__write_string(&file->file, data);
	// Write key pair
	saim_file__offset_from_beginning(&file->file, pair->info.key_offset);
	make_stored_key_pair(&stored_pair, pair);
	saim_storage_file__write_key_pair(file, &stored_pair);
	// Delete old key from list
	saim_map_nodes_list__delete(list, found_list_node);
	// Erase old key from map
	saim_key_offset_map__erase(offsets, found_node);
	// Insert a new key pair to map
	found_node = saim_key_offset_map__insert(offsets, pair);
	// Add a new key to list
	saim_map_nodes_list__insert(list, found_node);
	return kSave_Success;
}
save_result_t saim_storage_file__save_logics(saim_storage_file * file,
	const saim_data_key * key, const saim_string * data,
	saim_key_offset_map * offsets, saim_map_nodes_list * list)
{
	file_offset_t header_offset;
	file_offset_t block_header_size;
	file_offset_t block_keys_size;
	file_offset_t data_offset;
	file_offset_t data_size;
	file_offset_t this_block_offset;
	file_offset_t this_offset;
	file_offset_t file_size;
	file_offset_t key_displacement;
	unsigned int block;
	saim_set_node * found_node;
	key_pair_t * pair;
	stored_key_pair_t stored_pair;
	block_header_t block_header;
	block_keys_t keys;

	// Skip file header
	saim_file__offset_from_beginning(&file->file, kHeaderSize);
	header_offset = kHeaderSize;

	if (file->blocks_count == 0U)
	{
		block_header_size = (file_offset_t)(sizeof(block_header_t));
		block_keys_size = (file_offset_t)(sizeof(block_keys_t));
		data_offset = header_offset + block_header_size + block_keys_size;
		// Prepare out data
		pair = (key_pair_t *)SAIM_MALLOC(sizeof(key_pair_t));
		pair->key = *key;
		pair->info.key_offset = header_offset + block_header_size; // cuz 1st key
		pair->info.data_offset = data_offset;
		pair->info.size = data->length;
		pair->info.counter = 0U;
		// Insert pair into the map
		found_node = saim_key_offset_map__insert(offsets, pair);
		// Add a new key to list
		saim_map_nodes_list__insert(list, found_node);
		// Create our first block and write a key in there
		block_header.count = 1U;
		block_header.next_offset = 0;
		saim_storage_file__write_block_header(file, &block_header);
		memset(&keys, 0, sizeof(keys));
		keys.key_offsets[0].key = *key;
		keys.key_offsets[0].info.offset = data_offset;
		keys.key_offsets[0].info.size = data->length;
		keys.key_offsets[0].info.counter = 0U;
		saim_storage_file__write_block_keys(file, &keys);
		saim_file__write_array(&file->file, data->data, data->length);
		// Update blocks count information in header
		++file->blocks_count;
		saim_file__offset_from_beginning(&file->file, 0);
		saim_storage_file__write_header(file);
		return kSave_Success;
	}
	else // there are created blocks
	{
		data_size = (file_offset_t)(data->length);

		// Try to find first empty spot in each block
		for (block = 0; block < file->blocks_count; ++block)
		{
			// Store position
			this_block_offset = saim_file__tell(&file->file);

			// Read count of used keys in this block
			saim_storage_file__read_block_header(file, &block_header);

			// Check whether block is full
			if (block_header.count == SAIM_FILE_BLOCK_CAPACITY)
			{
				if (block_header.next_offset == 0) // it's the last block
				{
					// We've reached the maximum size of block, create a new one
					saim_file__offset_from_end(&file->file, 0); // seek to the end of file
					file_size = saim_file__tell(&file->file);
					block_header_size = (file_offset_t)(sizeof(block_header_t));
					block_keys_size = (file_offset_t)(sizeof(block_keys_t));
					data_offset = file_size + block_header_size + block_keys_size;
					if (data_offset + data_size > file->max_file_size)
					{
						return saim_storage_file__replace_key(file, key, data, offsets, list);
					}
					else
					{
						// Prepare out data
						pair = (key_pair_t *)SAIM_MALLOC(sizeof(key_pair_t));
						pair->key = *key;
						pair->info.key_offset = file_size + block_header_size; // cuz 1st key
						pair->info.data_offset = data_offset;
						pair->info.size = data->length;
						pair->info.counter = 0U;
						// Insert pair into the map
						found_node = saim_key_offset_map__insert(offsets, pair);
						// Add a new key to list
						saim_map_nodes_list__insert(list, found_node);
						// Write a new key in the new block
						block_header.count = 1U;
						block_header.next_offset = 0;
						saim_storage_file__write_block_header(file, &block_header);
						memset(&keys, 0, sizeof(keys));
						keys.key_offsets[0].key = *key;
						keys.key_offsets[0].info.offset = data_offset;
						keys.key_offsets[0].info.size = data->length;
						keys.key_offsets[0].info.counter = 0U;
						saim_storage_file__write_block_keys(file, &keys);
						saim_file__write_array(&file->file, data->data, data->length);
						// Also we need to store offset to this block in the last block
						saim_file__offset_from_beginning(&file->file, this_block_offset);
						block_header.count = SAIM_FILE_BLOCK_CAPACITY;
						block_header.next_offset = file_size;
						saim_storage_file__write_block_header(file, &block_header);
						// Update blocks count information in header
						++file->blocks_count;
						saim_file__offset_from_beginning(&file->file, 0);
						saim_storage_file__write_header(file);
						return kSave_Success;
					}
				}
				else
				{
					// There is another block, goto it
					saim_file__offset_from_beginning(&file->file, block_header.next_offset);
					continue;
				}
			}
			else // block isn't full
			{
				assert(block_header.next_offset == 0);
				// Seek to the end of file to get offset
				this_offset = saim_file__tell(&file->file);
				saim_file__offset_from_end(&file->file, 0);
				data_offset = saim_file__tell(&file->file);
				saim_file__offset_from_beginning(&file->file, this_offset);
				if (data_offset + data_size > file->max_file_size)
				{
					return saim_storage_file__replace_key(file, key, data, offsets, list);
				}
				else
				{
					// We will use summ of sizes to not use pragma pack
					key_displacement = block_header.count * sizeof(stored_key_pair_t);
					// Prepare out data
					pair = (key_pair_t *)SAIM_MALLOC(sizeof(key_pair_t));
					pair->key = *key;
					pair->info.key_offset = this_offset + key_displacement;
					pair->info.data_offset = data_offset;
					pair->info.size = data->length;
					pair->info.counter = 0U;
					// Insert pair into the map
					found_node = saim_key_offset_map__insert(offsets, pair);
					// Add a new key to list
					saim_map_nodes_list__insert(list, found_node);
					// Write key-offset pair
					saim_file__offset_from_current(&file->file, key_displacement);
					make_stored_key_pair(&stored_pair, pair);
					saim_storage_file__write_key_pair(file, &stored_pair);
					// Write data
					saim_file__offset_from_beginning(&file->file, data_offset);
					saim_file__write_array(&file->file, data->data, data->length);
					// Now we should increase block header count
					++block_header.count;
					saim_file__offset_from_beginning(&file->file, this_block_offset);
					saim_storage_file__write_block_header(file, &block_header);
					return kSave_Success;
				}
			}
		} // end for
		assert(false);
	}
	return kSave_Failed;
}