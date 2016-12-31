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

#ifndef __SAIM_REGIONS_HEADER_FILE_H__
#define __SAIM_REGIONS_HEADER_FILE_H__

#include "saim_file.h"
#include "saim_region_map.h"

typedef struct {
	saim_file file;
	unsigned int unique_id;		//!< unique region identifier (counter)
	unsigned int num_regions;	//!< number of regions
} saim_regions_header_file;

void saim_regions_header_file__create(saim_regions_header_file * file, const char* filename);
void saim_regions_header_file__destroy(saim_regions_header_file * file);

bool saim_regions_header_file__add_region(saim_regions_header_file * file, saim_region_map * map);
bool saim_regions_header_file__remove_region(saim_regions_header_file * file, saim_region_map * map);
bool saim_regions_header_file__update(saim_regions_header_file * file, saim_region_map * map); //!< simply rewrite entire file
void saim_regions_header_file__read_all_regions(saim_regions_header_file * file, saim_region_map * map);

bool saim_regions_header_file__flush(saim_regions_header_file * file);
void saim_regions_header_file__regenerate(saim_regions_header_file * file);

void saim_regions_header_file__mark_operations_begin(saim_regions_header_file * file);
void saim_regions_header_file__mark_operations_end(saim_regions_header_file * file);

// Write operations
void saim_regions_header_file__write_header(saim_regions_header_file * file);
void saim_regions_header_file__write_region_info(saim_regions_header_file * file, const saim_region_info * region);
void saim_regions_header_file__write_region_stored_info(saim_regions_header_file * file, const saim_stored_region_info * region);

// Read operations
bool saim_regions_header_file__read_header(saim_regions_header_file * file);
void saim_regions_header_file__read_region_info(saim_regions_header_file * file, saim_region_info * region);
void saim_regions_header_file__read_region_stored_info(saim_regions_header_file * file, saim_stored_region_info * region);


#endif