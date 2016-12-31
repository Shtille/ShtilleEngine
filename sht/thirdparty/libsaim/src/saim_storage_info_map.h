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

#ifndef __SAIM_STORAGE_INFO_MAP_H__
#define __SAIM_STORAGE_INFO_MAP_H__

#include "saim_storage_info_pair.h"
#include "util/saim_set.h"

typedef struct {
	saim_set * set;
} saim_storage_info_map;

void saim_storage_info_map__create(saim_storage_info_map * map);
void saim_storage_info_map__destroy(saim_storage_info_map * map);

void saim_storage_info_map__clear(saim_storage_info_map * map);
saim_set_node * saim_storage_info_map__insert(saim_storage_info_map * map, saim_storage_info_pair * pair);
void saim_storage_info_map__erase(saim_storage_info_map * map, saim_set_node * node);
void saim_storage_info_map__cut(saim_storage_info_map * map, saim_set_node * node);
saim_set_node * saim_storage_info_map__search(saim_storage_info_map * map, const saim_string * name);

#endif