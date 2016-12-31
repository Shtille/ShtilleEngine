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

#ifndef __SAIM_MAP_NODES_LIST_H__
#define __SAIM_MAP_NODES_LIST_H__

#include "util/saim_list.h"
#include "util/saim_set.h"
#include "saim_key_pair.h"

/*
* List stores set nodes.
*/

typedef struct {
	saim_list list;
} saim_map_nodes_list;

void saim_map_nodes_list__create(saim_map_nodes_list * list);
void saim_map_nodes_list__destroy(saim_map_nodes_list * list);

void saim_map_nodes_list__clear(saim_map_nodes_list * list);
void saim_map_nodes_list__insert(saim_map_nodes_list * list, saim_set_node * node);
void saim_map_nodes_list__delete(saim_map_nodes_list * list, saim_list_node * node);
void saim_map_nodes_list__sort(saim_map_nodes_list * list);
saim_list_node * saim_map_nodes_list__find_size_compatible(saim_map_nodes_list * list, file_size_t size);
saim_list_node * saim_map_nodes_list__get_first(saim_map_nodes_list * list);

#endif