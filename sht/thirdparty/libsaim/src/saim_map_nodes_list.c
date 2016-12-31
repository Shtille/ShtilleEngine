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

#include "saim_map_nodes_list.h"

#include "util/saim_memory.h"

#include <stdlib.h>
#include <assert.h>

static void key_destroy_func(void* p)
{
	// List stores set nodes, thus there's no need in clearing it
}
void saim_map_nodes_list__create(saim_map_nodes_list * list)
{
	saim_list_create(&list->list, key_destroy_func);
}
void saim_map_nodes_list__destroy(saim_map_nodes_list * list)
{
	saim_list_destroy(&list->list);
}
void saim_map_nodes_list__clear(saim_map_nodes_list * list)
{
	saim_list_clear(&list->list);
}
void saim_map_nodes_list__insert(saim_map_nodes_list * list, saim_set_node * node)
{
	saim_list_push_front(&list->list, node);
}
void saim_map_nodes_list__delete(saim_map_nodes_list * list, saim_list_node * node)
{
	/* We don't need to free data that has been popped from list,
	because it simply stores existing set node. */
	(void)saim_list_delete(&list->list, node);
}
static bool key_sort_func(const void * n1, const void * n2)
{
	const saim_set_node * node1 = (saim_set_node *)n1;
	const saim_set_node * node2 = (saim_set_node *)n2;
	const key_pair_t * pair1 = (const key_pair_t *)node1->data;
	const key_pair_t * pair2 = (const key_pair_t *)node2->data;
	return pair1->info.counter < pair2->info.counter;
}
void saim_map_nodes_list__sort(saim_map_nodes_list * list)
{
	saim_list_sort(&list->list, key_sort_func);
}
static bool size_fit_func(const void* s, const void* n)
{
	const file_size_t * size = (const file_size_t *)s;
	const saim_set_node * node = (saim_set_node *)n;
	const key_pair_t * pair = (const key_pair_t *)node->data;
	return *size <= pair->info.size;
}
saim_list_node * saim_map_nodes_list__find_size_compatible(saim_map_nodes_list * list, file_size_t size)
{
	return saim_list_find(&list->list, &size, size_fit_func);
}
saim_list_node * saim_map_nodes_list__get_first(saim_map_nodes_list * list)
{
	return list->list.head;
}